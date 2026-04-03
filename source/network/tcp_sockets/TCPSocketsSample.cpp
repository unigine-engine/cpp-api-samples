// Demonstrates TCP socket networking with client-server architecture.
// Server accepts multiple client connections and broadcasts camera state.
// Clients receive camera updates and sync their view to the server's position.
// Messages are serialized with header (type + size) followed by payload data.

#include "TCPSocketsSample.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>

#include "../../utils/Utils.h"

using namespace Unigine;

REGISTER_COMPONENT(TCPSocketsSample);

namespace
{
	constexpr const char *DEFAULT_SERVER_HOSTNAME = "127.0.0.1";
	constexpr unsigned short DEFAULT_SERVER_PORT = 64'000;

	constexpr int SEND_BUFFER_SIZE = 4096;
	constexpr int RECV_BUFFER_SIZE = 4096;

	constexpr int MAX_CLIENT_CONNECTIONS = 8;

	constexpr int CLIENT_CONNECTION_TIMEOUT_MS = 10'000;
	constexpr int CLIENT_WRITE_TIMEOUT_US = 1'000;
	constexpr int CLIENT_READ_TIMEOUT_US = 1'000;
}

// GUI is initialized; host (Client or Server) is created via button click.
void TCPSocketsSample::init()
{
	gui.init(this);
}

// Active host (Client or Server) is updated; GUI reflects connection state.
void TCPSocketsSample::update()
{
	if (host)
		host->update();

	gui.update();
}

// Active host is destroyed and GUI is cleaned up.
void TCPSocketsSample::shutdown()
{
	if (host)
	{
		delete host;
		host = nullptr;
	}

	gui.shutdown();
}

// =================================================================================
// CLIENT IMPLEMENTATION
// =================================================================================

// TCP socket is created, configured as non-blocking, and network thread is started.
TCPSocketsSample::Client::Client(const Unigine::String &hostname, unsigned short port)
{
	// Socket is created with target server address for subsequent connect() call.
	// Note: socket's own address is assigned automatically by the OS.
	SocketPtr socket = Socket::create(Socket::SOCKET_TYPE_STREAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return;
	}

	// Send and receive buffer sizes are configured
	socket->send(SEND_BUFFER_SIZE);
	socket->recv(RECV_BUFFER_SIZE);

	// Non-blocking mode allows polling without stalling the thread
	socket->nonblock();

	thread.start(socket, true);

	// Player control is disabled so camera can be synced from server
	if (PlayerPtr player = Game::getPlayer())
		player->setControlled(false);

	Console::addCommand(
		"send_msg",
		"[Network Sockets Sample] Send a text message to peer.",
		MakeCallback(this, &TCPSocketsSample::Client::on_message_send_cmd)
	);
}

// Player control is restored and console command is unregistered.
TCPSocketsSample::Client::~Client()
{
	if (PlayerPtr player = Game::getPlayer())
		player->setControlled(true);

	Console::removeCommand("send_msg");
}

// Incoming messages from server are processed; camera is synced on CameraMessage.
void TCPSocketsSample::Client::update()
{
	if (!thread.isRunning())
		return;

	Message *message = thread.receive();
	int processed = 0;

	// Messages are processed up to a limit per frame to avoid stalls
	while (message && processed < message_process_limit)
	{
		switch (message->getType())
		{
			case Message::TEXT:
			{
				// Text message is logged to console
				TextMessage *text_msg = dynamic_cast<TextMessage *>(message);

				if (text_msg->text.get() && !text_msg->text.empty())
					Log::message("Received a text message from Server: %s\n", text_msg->text.get());

				break;
			}

			case Message::CAMERA:
			{
				// Camera transform is applied to local player
				CameraMessage *camera_msg = dynamic_cast<CameraMessage *>(message);

				if (camera_msg)
				{
					PlayerPtr player = Game::getPlayer();

					if (player)
					{
						player->setWorldPosition(camera_msg->position);
						player->setWorldRotation(camera_msg->rotation);
					}
				}

				break;
			}
		}

		delete message;
		message = thread.receive();
		processed += 1;
	}
}

// Network thread is started with provided socket; optional connect flag for clients.
void TCPSocketsSample::Client::NetworkThread::start(Unigine::SocketPtr socket, bool connect)
{
	reset();
	this->socket = socket;
	this->connect = connect;
	run();
}

// Thread is stopped, socket is closed, and message queues are cleared.
void TCPSocketsSample::Client::NetworkThread::reset()
{
	shutdown();

	if (socket)
	{
		socket->close();
		socket.clear();
	}

	send_queue.clear();
	recv_queue.clear();
}

// Message is queued for sending on the network thread.
void TCPSocketsSample::Client::NetworkThread::send(Message *message)
{
	send_queue.push(message);
}

// Next received message is returned; nullptr if queue is empty.
TCPSocketsSample::Message *TCPSocketsSample::Client::NetworkThread::receive()
{
	return recv_queue.pop();
}

// Main network loop handling bidirectional TCP message streaming.
// TCP is stream-oriented, so message boundaries are tracked via state machine:
// RECEIVE_HEADER -> RECEIVE_PAYLOAD -> UNPACK_MESSAGE -> repeat.
// Protocol format: [Header(type:4B, size:4B)] [Payload(variable)].
void TCPSocketsSample::Client::NetworkThread::process()
{
	enum { RECEIVE_HEADER, RECEIVE_PAYLOAD, UNPACK_MESSAGE } recv_state = RECEIVE_HEADER;

	int recv_size = 0;
	int send_size = 0;

	BlobPtr send_blob = Blob::create();
	BlobPtr recv_blob = Blob::create();

	// Perform the connection on the socket if asked to (only used by Client).
	if (connect)
	{
		if (socket->connect(CLIENT_CONNECTION_TIMEOUT_MS))
		{
			Log::message("[Client::NetworkThread] Successfully connected to the server (%s:%d).\n", socket->getHost(), socket->getPort());
		}
		else
		{
			Log::warning("[Client::NetworkThread] Could not connect to the server at the specified address (%s:%d)!\n", socket->getHost(), socket->getPort());
			return;
		}
	}

	while (isRunning())
	{
		// ==================== Send outgoing messages ====================

		{
			if (send_blob->getSize() == 0)
			{
				Message *send_message = send_queue.pop();
				if (send_message)
				{
					send_size = send_message->pack(send_blob);
					send_blob->seekSet(0);

					delete send_message;
					send_message = nullptr;
				}
			}

			if (send_blob->getSize())
			{
				if (socket->isReadyToWrite(CLIENT_WRITE_TIMEOUT_US))
				{
					int written = socket->writeStream(send_blob, send_size);

					if (written != send_size)
					{
						// Could not send the message in full. Something must've gone wrong with the connection.

						Log::message("[Client::NetworkThread] The remote connection was closed or an error has occured. Closing the socket.\n");

						socket->close();
						return;
					}

					send_blob->clear();
				}
			}
		}

		// ==================== Receive incoming messages ====================

		{
			switch (recv_state)
			{
				case RECEIVE_HEADER:
				{
					// Going to read the next message's header.

					recv_blob->resize(sizeof(Message::Header));
					recv_blob->seekSet(0);

					recv_size = sizeof(Message::Header);
					break;
				}

				case RECEIVE_PAYLOAD:
				{
					// Received the message header, now going to read the payload following it.
					// Here we unpack the header, check it for validity, and save the message payload size.

					Message::Header header;

					recv_blob->seekSet(0);
					recv_blob->read(&header, sizeof(Message::Header));

					// We don't reset the blob cursor back to start here after Blob::read
					// so that the next call to Socket::recvStream with this blob
					// does not overwrite the header.

					int type = header.type;
					int size = header.size;

					bool valid = (size >= 0 && size <= RECV_BUFFER_SIZE) && (type >= 0 && type < Message::TYPE::NUM_TYPES);

					if (valid)
					{
						recv_blob->resize(size);
						recv_size = Math::max(0, size - int(sizeof(Message::Header)));
					}
					else
					{
						// The message size or type are invalid.

						// This is probably because we somehow lost track of where messages start and end
						// in the incoming data stream. Our position in the stream is now undetermined,
						// and there isn't much we can do at this point. Close the connection.

						if (size > RECV_BUFFER_SIZE)
							Log::message("[Client::NetworkThread] The message payload size was too large (%d)!\n", size);

						if (type < 0 || type >= Message::TYPE::NUM_TYPES)
							Log::message("[Client::NetworkThread] The message type was invalid (%d)!\n", type);

						Log::message("[Client::NetworkThread] Closing the connection.\n");
						socket->close();
						return;
					}

					break;
				}

				case UNPACK_MESSAGE:
				{
					// Received the message in full. Unpack it and store in the queue.

					recv_blob->seekSet(0); // Reset the blob cursor the start so that we read the whole message, including the header.
					Message *message = extract_message(recv_blob);

					if (message)
						recv_queue.push(message);

					recv_state = RECEIVE_HEADER;
					recv_blob->clear();
					recv_size = 0;

					break;
				}
			}

			if (recv_size > 0)
			{
				// Read the next portion (header or payload) of the message from the socket.

				if (socket->isReadyToRead(CLIENT_READ_TIMEOUT_US))
				{
					int read = socket->readStream(recv_blob, recv_size);

					if (read == 0 || read != recv_size)
					{
						// Connection has been closed/reset/terminated or an error has occured.

						Log::message("[Client::NetworkThread] The remote connection was terminated or an error has occured. Closing the socket.\n");

						socket->close();
						return;
					}

					switch (recv_state)
					{
						case RECEIVE_HEADER: recv_state = RECEIVE_PAYLOAD; break;
						case RECEIVE_PAYLOAD: recv_state = UNPACK_MESSAGE; break;
						default: break;
					}
				}
			}
		}
	}
}

// Message is created based on header type and deserialized from blob.
TCPSocketsSample::Message *TCPSocketsSample::Client::NetworkThread::extract_message(Unigine::BlobPtr blob)
{
	Message *message = nullptr;
	Message::Header header;

	blob->read(&header, sizeof(Message::Header));
	blob->seekSet(0);

	// Appropriate message subclass is instantiated based on type
	switch (header.type)
	{
		case Message::TEXT: message = new TextMessage(); break;
		case Message::CAMERA: message = new CameraMessage(); break;
		default: message = nullptr; break;
	}

	if (message)
		message->unpack(blob);

	return message;
}

// Console command handler: text message is sent to the server.
void TCPSocketsSample::Client::on_message_send_cmd(int argc, char **argv)
{
	StringStack<> text;

	// Arguments are concatenated into a single message string
	for (int i = 1; i < argc; i += 1)
	{
		text += argv[i];
		if (i != (argc - 1))
			text += " ";
	}

	thread.send(new TextMessage(text));
}

// =================================================================================
// SERVER IMPLEMENTATION
// =================================================================================

// Listen socket is created, bound to address, and accept thread is started.
TCPSocketsSample::Server::Server(const Unigine::String &hostname, unsigned short port)
{
	SocketPtr socket = Socket::create(Socket::SOCKET_TYPE_STREAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return;
	}

	// Non-blocking mode for accept operations
	socket->nonblock();

	// Socket is bound to the specified address
	if (!socket->bind())
	{
		Log::warning("Could not bind socket to the specified address (%s:%d)!\n", hostname.get(), int(port));
		return;
	}

	// Socket is set to listen mode with connection backlog limit
	socket->listen(MAX_CLIENT_CONNECTIONS);

	clients.reserve(MAX_CLIENT_CONNECTIONS);

	thread.start(socket);

	Console::addCommand(
		"send_msg",
		"[Network Sockets Sample] Send a text message to peer.",
		MakeCallback(this, &TCPSocketsSample::Server::on_message_send_cmd)
	);
}

// All client threads are destroyed and console command is unregistered.
TCPSocketsSample::Server::~Server()
{
	for (int i = 0; i < clients.size(); i += 1)
	{
		if (Client::NetworkThread *client = clients[i])
			delete client;
	}

	Console::removeCommand("send_msg");
}

// Disconnected clients are removed; new connections are accepted; messages are exchanged.
void TCPSocketsSample::Server::update()
{
	// Terminated client connections are cleaned up
	for (int i = 0; i < clients.size(); i += 1)
	{
		Client::NetworkThread *client = clients[i];

		if (!client->isConnectionActive())
		{
			clients.removeFast(i--);
			delete client;
		}
	}

	// New connections are accepted from the listen thread's queue
	while (SocketPtr connection = thread.accept())
	{
		Client::NetworkThread *client = new Client::NetworkThread(connection, false);
		Log::message("[Server] Accepted a client connection (%s:%d).\n", client->getHostname(), client->getPort());
		clients.push_back(client);
	};

	// Messages are exchanged with each connected client
	for (int i = 0; i < clients.size(); i += 1)
	{
		Client::NetworkThread *client = clients[i];

		// Incoming text messages are logged
		if (Message *message = client->receive())
		{
			TextMessage *text_msg = dynamic_cast<TextMessage *>(message);
			if (text_msg && text_msg->text.get() && !text_msg->text.empty())
				Log::message("Received a text message from Client [%d]: %s\n", i, text_msg->text.get());
		}

		// Camera transform is broadcast to all clients each frame
		if (PlayerPtr player = Game::getPlayer())
			client->send(new CameraMessage(player->getWorldPosition(), player->getWorldRotation()));
	}
}

// Accept thread is started with the listen socket.
void TCPSocketsSample::Server::NetworkThread::start(Unigine::SocketPtr socket)
{
	reset();

	this->socket = socket;

	run();
}

// Thread is stopped and listen socket is closed.
void TCPSocketsSample::Server::NetworkThread::reset()
{
	shutdown();

	if (socket)
	{
		socket->close();
		socket.clear();
	}
}

// Next accepted connection is returned; nullptr if queue is empty.
SocketPtr TCPSocketsSample::Server::NetworkThread::accept()
{
	ScopedLock lock(mutex);
	SocketPtr connection;

	if (connections.size())
		connection = connections.takeFirst();

	return connection;
}

// Incoming connections are accepted and queued for main thread processing.
void TCPSocketsSample::Server::NetworkThread::process()
{
	while (isRunning())
	{
		SocketPtr client = Socket::create(Socket::SOCKET_TYPE_STREAM);

		// Accepted connection is added to thread-safe queue
		if (socket->accept(client))
		{
			ScopedLock lock(mutex);
			connections.push_back(client);
		}
	}
}

// Console command handler: text message is broadcast to all connected clients.
void TCPSocketsSample::Server::on_message_send_cmd(int argc, char **argv)
{
	StringStack<> text;

	for (int i = 1; i < argc; i += 1)
	{
		text += argv[i];
		if (i != (argc - 1))
			text += " ";
	}

	// Message is sent to every connected client
	for (int i = 0; i < clients.size(); i += 1)
		clients[i]->send(new TextMessage(text));
}

// =================================================================================
// MESSAGE SERIALIZATION
// =================================================================================

// TextMessage is serialized: header + text_size + text_data.
size_t TCPSocketsSample::TextMessage::pack(Unigine::BlobPtr dst_blob)
{
	size_t cursor = dst_blob->tell();
	size_t packed = 0;

	// Header is written first (will be updated with final size)
	packed += dst_blob->write(&header, sizeof(header));

	// Text length is written followed by raw string data
	size_t text_size = text.size();
	packed += dst_blob->write(&text_size, sizeof(text_size));
	packed += dst_blob->write(text.get(), text_size);

	// Header is updated with actual type and size
	header.type = getType();
	header.size = packed;

	dst_blob->seekSet(cursor);
	dst_blob->write(&header, sizeof(header));

	dst_blob->seekSet(cursor + packed);

	return packed;
}

// TextMessage is deserialized from blob.
size_t TCPSocketsSample::TextMessage::unpack(Unigine::BlobPtr src_blob)
{
	size_t unpacked = 0;

	unpacked += src_blob->read(&header, sizeof(header));

	size_t text_size = 0;

	unpacked += src_blob->read(&text_size, sizeof(text_size));

	if (text_size)
	{
		text.resize(text_size);
		unpacked += src_blob->read(text.getRaw(), text_size);
	}

	return unpacked;
}

// CameraMessage is serialized: header + position (Vec3) + rotation (quat).
size_t TCPSocketsSample::CameraMessage::pack(Unigine::BlobPtr dst_blob)
{
	size_t cursor = dst_blob->tell();
	size_t packed = 0;

	packed += dst_blob->write(&header, sizeof(header));
	packed += dst_blob->write(&position, sizeof(position));
	packed += dst_blob->write(&rotation, sizeof(rotation));

	header.type = getType();
	header.size = packed;

	dst_blob->seekSet(cursor);
	dst_blob->write(&header, sizeof(header));

	dst_blob->seekSet(cursor + packed);

	return packed;
}

// CameraMessage is deserialized from blob.
size_t TCPSocketsSample::CameraMessage::unpack(Unigine::BlobPtr src_blob)
{
	size_t unpacked = 0;

	unpacked += src_blob->read(&header, sizeof(header));
	unpacked += src_blob->read(&position, sizeof(position));
	unpacked += src_blob->read(&rotation, sizeof(rotation));

	return unpacked;
}

// =================================================================================
// GUI IMPLEMENTATION
// =================================================================================

// UI window is created with Client/Server buttons and address input fields.
void TCPSocketsSample::SampleGui::init(TCPSocketsSample *sample)
{
	this->sample = sample;
	is_onscreen = Console::isOnscreen();
	Console::setOnscreen(true);

	sample_description_window.createWindow(Gui::ALIGN_RIGHT);

	auto const &group_box = sample_description_window.getParameterGroupBox();

	auto vbox = WidgetVBox::create();
	auto buttons_hbox = WidgetHBox::create();

	start_client_btn = WidgetButton::create("Client");
	start_client_btn->setToggleable(true);
	start_client_btn->setToggled(false);
	start_client_btn->setHeight(24);
	start_client_btn->getEventClicked().connect(this, &SampleGui::on_start_client_btn_clicked);
	buttons_hbox->addChild(start_client_btn);

	start_server_btn = WidgetButton::create("Server");
	start_server_btn->setToggleable(true);
	start_server_btn->setToggled(false);
	start_server_btn->setHeight(24);
	start_server_btn->getEventClicked().connect(this, &SampleGui::on_start_server_btn_clicked);
	buttons_hbox->addChild(start_server_btn);

	auto spacer = WidgetSpacer::create();
	spacer->setOrientation(0);
	buttons_hbox->addChild(spacer);

	server_hostname_el = WidgetEditLine::create(DEFAULT_SERVER_HOSTNAME);
	server_hostname_el->setWidth(100);
	server_hostname_el->setHeight(18);
	server_hostname_el->setFontVOffset(-2);
	buttons_hbox->addChild(server_hostname_el);

	buttons_hbox->addChild(WidgetLabel::create(":"));

	server_port_el = WidgetEditLine::create(String::itoa(DEFAULT_SERVER_PORT).get());
	server_port_el->setWidth(45);
	server_port_el->setHeight(18);
	server_port_el->setFontVOffset(-2);
	server_port_el->setValidator(Gui::VALIDATOR_UINT);
	buttons_hbox->addChild(server_port_el);

	vbox->addChild(buttons_hbox);
	vbox->addChild(WidgetVBox::create(0, 5));

	client_connections_gbox = WidgetGroupBox::create("", 0, 5);
	vbox->addChild(client_connections_gbox);

	group_box->addChild(vbox, Gui::ALIGN_TOP);
	group_box->arrange();

	for (int i = 0; i < Engine::get()->getNumArgs(); i++)
	{
		String str = Engine::get()->getArg(i);
		if (str == "-server")
			start_server_btn->setToggled(true);
		else if (str == "-client")
			start_client_btn->setToggled(true);
	}
}

// Connection list is refreshed to show active client/server connections.
void TCPSocketsSample::SampleGui::update()
{
	auto add_connection_gui = [this](const char *hostname, unsigned short port)
	{
		auto hbox = WidgetHBox::create();
		hbox->addChild(WidgetLabel::create(String::format("%s:%d", hostname, int(port)).get()));
		client_connections_gbox->addChild(hbox);
	};

	// Previous connection entries are cleared
	while (client_connections_gbox->getNumChildren())
	{
		auto child = client_connections_gbox->getChild(0);
		client_connections_gbox->removeChild(child);
	}

	// Client mode: single connection to server is displayed
	if (auto client = dynamic_cast<TCPSocketsSample::Client *>(sample->host))
	{
		if (client->isConnectionActive())
			add_connection_gui(client->getHostname(), client->getPort());
	}

	// Server mode: all connected clients are listed
	else if (auto server = dynamic_cast<TCPSocketsSample::Server *>(sample->host))
	{
		auto const& clients = server->getClients();
		for (int i = 0; i < clients.size(); i += 1)
			add_connection_gui(clients[i]->getHostname(), clients[i]->getPort());
	}
}

// UI is cleaned up and console state is restored.
void TCPSocketsSample::SampleGui::shutdown()
{
	sample_description_window.shutdown();
	Console::setOnscreen(is_onscreen);
	sample = nullptr;
}

// Server mode is started; previous host is destroyed if exists.
void TCPSocketsSample::SampleGui::on_start_server_btn_clicked(const WidgetPtr &widget, int mouse)
{
	if (sample->host)
	{
		delete sample->host;
		sample->host = nullptr;
	}

	MUTE_EVENT(start_server_btn->getEventClicked());
	MUTE_EVENT(start_client_btn->getEventClicked());

	if (start_server_btn->isToggled())
	{
		// Server is created with address from input fields
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = server_hostname_el->getText();
		port = (unsigned short) String::atoi(server_port_el->getText());

		sample->host = new Server(hostname, port);

		// Address fields are locked while server is running
		server_hostname_el->setEnabled(false);
		server_port_el->setEnabled(false);

		start_client_btn->setToggled(false);
	}

	else
	{
		server_hostname_el->setEnabled(true);
		server_port_el->setEnabled(true);
	}
}

// Client mode is started; connection to server is initiated.
void TCPSocketsSample::SampleGui::on_start_client_btn_clicked(const WidgetPtr &widget, int mouse)
{
	if (sample->host)
	{
		delete sample->host;
		sample->host = nullptr;
	}

	MUTE_EVENT(start_server_btn->getEventClicked());
	MUTE_EVENT(start_client_btn->getEventClicked());

	if (start_client_btn->isToggled())
	{
		// Client connects to server at specified address
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = server_hostname_el->getText();
		port = (unsigned short) String::atoi(server_port_el->getText());

		sample->host = new Client(hostname, port);

		// Address fields are locked while client is connected
		server_hostname_el->setEnabled(false);
		server_port_el->setEnabled(false);

		start_server_btn->setToggled(false);
	}

	else
	{
		server_hostname_el->setEnabled(true);
		server_port_el->setEnabled(true);
	}
}
