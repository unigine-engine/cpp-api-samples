// Demonstrates UDP socket networking for peer-to-peer communication. Sender
// broadcasts camera state to a receiver at fixed address. Receiver syncs its
// view to received camera data. Message protocol uses type+size headers.

#include "UDPSocketsSample.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>

using namespace Unigine;

REGISTER_COMPONENT(UDPSocketsSample);

namespace
{
	constexpr const char *DEFAULT_RECEIVER_HOSTNAME = "127.0.0.1";
	constexpr unsigned short DEFAULT_RECEIVER_PORT = 64'000;

	constexpr int SEND_BUFFER_SIZE = 4096;
	constexpr int RECV_BUFFER_SIZE = 4096;
}

// GUI is initialized; peer (Sender or Receiver) is created via button click.
void UDPSocketsSample::init()
{
	gui.init(this);
}

// Active peer (Sender or Receiver) is updated each frame.
void UDPSocketsSample::update()
{
	if (peer)
		peer->update();
}

// Active peer is destroyed and GUI is cleaned up.
void UDPSocketsSample::shutdown()
{
	if (peer)
	{
		delete peer;
		peer = nullptr;
	}

	gui.shutdown();
}

// ================================================================
// SENDER IMPLEMENTATION
// ================================================================

// Network thread is started and console command is registered.
UDPSocketsSample::Sender::Sender(const Unigine::String &hostname, unsigned short port)
{
	thread.start(hostname, port);

	Console::addCommand(
		"send_msg",
		"[Network Sockets Sample] Send a text message to peer.",
		MakeCallback(this, &UDPSocketsSample::Sender::on_message_send_cmd)
	);
}

// Console command is unregistered.
UDPSocketsSample::Sender::~Sender()
{
	Console::removeCommand("send_msg");
}

// Camera transform is sent to receiver each frame.
void UDPSocketsSample::Sender::update()
{
	if (!thread.isRunning())
		return;

	// Current camera position and rotation are broadcast
	PlayerPtr player = Game::getPlayer();
	if (player)
	{
		thread.send(new CameraMessage(
			player->getWorldPosition(),
			player->getWorldRotation())
		);
	}
}

// Console command handler: text message is sent to receiver.
void UDPSocketsSample::Sender::on_message_send_cmd(int argc, char **argv)
{
	BlobPtr message = Blob::create();
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

// ================================================================

// UDP socket is created with target address; thread is started.
bool UDPSocketsSample::Sender::NetworkThread::start(const String &hostname, unsigned short port)
{
	// UDP socket is created - target address is used for subsequent send calls.
	// Socket's own address is assigned automatically by the OS.
	socket = Socket::create(Socket::SOCKET_TYPE_DGRAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return false;
	}

	// Send buffer size is configured
	socket->send(SEND_BUFFER_SIZE);

	run();
	return true;
}

// Message is queued for sending on the network thread.
void UDPSocketsSample::Sender::NetworkThread::send(Message *message)
{
	queue.push(message);
}

// Thread is stopped and socket is closed.
void UDPSocketsSample::Sender::NetworkThread::reset()
{
	shutdown();

	if (socket)
	{
		socket->close();
		socket = {};
	}

	queue.clear();
}

// Outgoing messages are serialized and sent as UDP datagrams.
void UDPSocketsSample::Sender::NetworkThread::process()
{
	BlobPtr blob = Blob::create();

	while (isRunning())
	{
		// Next message is retrieved from queue
		Message *message = queue.pop();

		if (message)
		{
			// Message is serialized into blob
			message->pack(blob);
			blob->seekSet(0);
			delete message;
		}

		// Datagram is sent to receiver
		if (blob->getSize())
		{
			socket->writeStream(blob, blob->getSize());
			blob->clear();
		}
	}
}

// ================================================================
// RECEIVER IMPLEMENTATION
// ================================================================

// Network thread is started; player control is disabled for camera sync.
UDPSocketsSample::Receiver::Receiver(const Unigine::String &hostname, unsigned short port)
{
	thread.start(hostname, port);

	// Player control is disabled so camera can be synced from sender
	PlayerPtr player = Game::getPlayer();
	if (player)
		player->setControlled(false);
}

// Player control is restored.
UDPSocketsSample::Receiver::~Receiver()
{
	PlayerPtr player = Game::getPlayer();
	if (player)
		player->setControlled(true);
}

// Incoming messages are processed; camera is synced on CameraMessage.
void UDPSocketsSample::Receiver::update()
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

				if (text_msg)
					Log::message("Received a text message from peer: \"%s\".\n", text_msg->text.get());

			} break;

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

			} break;

			default: break;
		}

		delete message;
		message = thread.receive();
		processed += 1;
	}
}

// ================================================================

// UDP socket is created, bound to address, and thread is started.
bool UDPSocketsSample::Receiver::NetworkThread::start(const String &hostname, unsigned short port)
{
	// UDP socket is created with local binding address
	socket = Socket::create(Socket::SOCKET_TYPE_DGRAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return false;
	}

	// Receive buffer size is configured
	socket->recv(RECV_BUFFER_SIZE);

	// Non-blocking mode for polling without stalling
	socket->nonblock();

	// Socket is bound to listen for incoming datagrams
	if (!socket->bind())
	{
		Log::warning("Could not bind socket to the specified address (%s:%d)!\n", hostname.get(), port);
		return false;
	}

	run();
	return true;
}

// Thread is stopped and socket is closed.
void UDPSocketsSample::Receiver::NetworkThread::reset()
{
	shutdown();

	if (socket)
	{
		socket->close();
		socket = nullptr;
	}

	queue.clear();
}

// Next received message is returned; nullptr if queue is empty.
UDPSocketsSample::Message *UDPSocketsSample::Receiver::NetworkThread::receive()
{
	return queue.pop();
}

// Incoming UDP datagrams are received and parsed into messages.
void UDPSocketsSample::Receiver::NetworkThread::process()
{
	BlobPtr blob = Blob::create();

	while (isRunning())
	{
		// Datagram is read from socket
		socket->readStream(blob, RECV_BUFFER_SIZE);
		blob->seekSet(0);

		// Message is extracted and queued for main thread
		if (blob->getSize())
		{
			Message *message = extract_message(blob);

			if (message)
				queue.push(message);
		}

		blob->clear();
	}
}

// Message is created based on header type and deserialized from blob.
UDPSocketsSample::Message *UDPSocketsSample::Receiver::NetworkThread::extract_message(Unigine::BlobPtr blob)
{
	Message::Header header = {};
	Message *message = nullptr;

	blob->read(&header, sizeof(header));
	blob->seekSet(0);

	// Appropriate message subclass is instantiated based on type
	switch (header.type)
	{
		case Message::TEXT: message = new TextMessage(); break;
		case Message::CAMERA: message = new CameraMessage(); break;
		default: break;
	}

	if (message)
		message->unpack(blob);

	return message;
}

// ================================================================
// MESSAGE SERIALIZATION
// ================================================================

// TextMessage is serialized: header + text_size + text_data.
size_t UDPSocketsSample::TextMessage::pack(Unigine::BlobPtr dst_blob)
{
	size_t cursor = dst_blob->tell();
	size_t packed = 0;

	packed += dst_blob->write(&header, sizeof(header));

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
size_t UDPSocketsSample::TextMessage::unpack(Unigine::BlobPtr src_blob)
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
size_t UDPSocketsSample::CameraMessage::pack(Unigine::BlobPtr dst_blob)
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
size_t UDPSocketsSample::CameraMessage::unpack(Unigine::BlobPtr src_blob)
{
	size_t unpacked = 0;

	unpacked += src_blob->read(&header, sizeof(header));
	unpacked += src_blob->read(&position, sizeof(position));
	unpacked += src_blob->read(&rotation, sizeof(rotation));

	return unpacked;
}

// ================================================================
// GUI IMPLEMENTATION
// ================================================================

// Sender mode is started; previous peer is destroyed if exists.
void UDPSocketsSample::SampleGui::on_start_sender_btn_clicked(const WidgetPtr &widget, int mouse)
{
	if (sample->peer)
	{
		delete sample->peer;
		sample->peer = nullptr;
	}

	// Button events are temporarily disabled to prevent reentrancy
	start_recver_btn->getEventClicked().setEnabled(false);
	start_sender_btn->getEventClicked().setEnabled(false);

	if (start_sender_btn->isToggled())
	{
		// Sender is created with target receiver address
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = recv_host_el->getText();
		port = (unsigned short) String::atoi(recv_port_el->getText());

		sample->peer = new Sender(hostname, port);

		// Address fields are locked while sender is running
		recv_host_el->setEnabled(false);
		recv_port_el->setEnabled(false);

		start_recver_btn->setToggled(false);
	}

	else
	{
		recv_host_el->setEnabled(true);
		recv_port_el->setEnabled(true);
	}

	start_recver_btn->getEventClicked().setEnabled(true);
	start_sender_btn->getEventClicked().setEnabled(true);
}

// Receiver mode is started; socket is bound to listen address.
void UDPSocketsSample::SampleGui::on_start_recver_btn_clicked(const WidgetPtr &widget, int mouse)
{
	if (sample->peer)
	{
		delete sample->peer;
		sample->peer = nullptr;
	}

	start_recver_btn->getEventClicked().setEnabled(false);
	start_sender_btn->getEventClicked().setEnabled(false);

	if (start_recver_btn->isToggled())
	{
		// Receiver is created and bound to specified address
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = recv_host_el->getText();
		port = (unsigned short) String::atoi(recv_port_el->getText());

		sample->peer = new Receiver(hostname, port);

		// Address fields are locked while receiver is running
		recv_host_el->setEnabled(false);
		recv_port_el->setEnabled(false);

		start_sender_btn->setToggled(false);
	}

	else
	{
		recv_host_el->setEnabled(true);
		recv_port_el->setEnabled(true);
	}

	start_recver_btn->getEventClicked().setEnabled(true);
	start_sender_btn->getEventClicked().setEnabled(true);
}

// UI window is created with Sender/Receiver buttons and address input fields.
void UDPSocketsSample::SampleGui::init(UDPSocketsSample *sample)
{
	this->sample = sample;
	is_onscreen = Console::isOnscreen();
	Console::setOnscreen(true);

	sample_description_window.createWindow(Gui::ALIGN_RIGHT);

	auto const &group_box = sample_description_window.getParameterGroupBox();

	auto hbox = WidgetHBox::create();
	group_box->addChild(hbox, Gui::ALIGN_TOP);

	start_sender_btn = WidgetButton::create("Sender");
	start_sender_btn->setToggleable(true);
	start_sender_btn->setToggled(false);
	start_sender_btn->setHeight(24);
	start_sender_btn->getEventClicked().connect(this, &SampleGui::on_start_sender_btn_clicked);
	hbox->addChild(start_sender_btn);

	start_recver_btn = WidgetButton::create("Receiver");
	start_recver_btn->setToggleable(true);
	start_recver_btn->setToggled(false);
	start_recver_btn->setHeight(24);
	start_recver_btn->getEventClicked().connect(this, &SampleGui::on_start_recver_btn_clicked);
	hbox->addChild(start_recver_btn);

	auto spacer = WidgetSpacer::create();
	spacer->setOrientation(0);
	hbox->addChild(spacer);

	recv_host_el = WidgetEditLine::create(DEFAULT_RECEIVER_HOSTNAME);
	recv_host_el->setWidth(100);
	recv_host_el->setHeight(18);
	recv_host_el->setFontVOffset(-2);
	hbox->addChild(recv_host_el);

	hbox->addChild(WidgetLabel::create(":"));

	recv_port_el = WidgetEditLine::create(String::itoa(DEFAULT_RECEIVER_PORT).get());
	recv_port_el->setWidth(45);
	recv_port_el->setHeight(18);
	recv_port_el->setFontVOffset(-2);
	recv_port_el->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(recv_port_el);

	for (int i = 0; i < Engine::get()->getNumArgs(); i++)
	{
		String str = Engine::get()->getArg(i);
		if (str == "-server")
			start_recver_btn->setToggled(true);
		else if (str == "-client")
			start_sender_btn->setToggled(true);
	}
}

// UI is cleaned up and console state is restored.
void UDPSocketsSample::SampleGui::shutdown()
{
	sample_description_window.shutdown();
	Console::setOnscreen(is_onscreen);
	sample = nullptr;
}
