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

void UDPSocketsSample::init()
{
	gui.init(this);
}

void UDPSocketsSample::update()
{
	if (peer)
		peer->update();
}

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

UDPSocketsSample::Sender::Sender(const Unigine::String &hostname, unsigned short port)
{
	thread.start(hostname, port);

	Console::addCommand(
		"send_msg",
		"[Network Sockets Sample] Send a text message to peer.",
		MakeCallback(this, &UDPSocketsSample::Sender::on_message_send_cmd)
	);
}

UDPSocketsSample::Sender::~Sender()
{
	Console::removeCommand("send_msg");
}

void UDPSocketsSample::Sender::update()
{
	if (!thread.isRunning())
		return;

	PlayerPtr player = Game::getPlayer();
	if (player)
	{
		thread.send(new CameraMessage(
			player->getWorldPosition(),
			player->getWorldRotation())
		);
	}
}

void UDPSocketsSample::Sender::on_message_send_cmd(int argc, char **argv)
{
	BlobPtr message = Blob::create();
	StringStack<> text;

	for (int i = 1; i < argc; i += 1)
	{
		text += argv[i];
		if (i != (argc - 1))
			text += " ";
	}

	thread.send(new TextMessage(text));
}

// ================================================================

bool UDPSocketsSample::Sender::NetworkThread::start(const String &hostname, unsigned short port)
{
	// Create a simple UDP socket.

	// Note that the given address (hostname:port) is used as the source address in subsequent
	// 'send' calls (Socket::write*). The socket's own address is set automatically.

	socket = Socket::create(Socket::SOCKET_TYPE_DGRAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return false;
	}

	// Set the socket's send buffer size.
	socket->send(SEND_BUFFER_SIZE);

	run();
	return true;
}

void UDPSocketsSample::Sender::NetworkThread::send(Message *message)
{
	queue.push(message);
}

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

void UDPSocketsSample::Sender::NetworkThread::process()
{
	// Takes messages from the outgoing messages queue and sends them to the peer.

	BlobPtr blob = Blob::create();

	while (isRunning())
	{
		Message *message = queue.pop();

		if (message)
		{
			message->pack(blob);
			blob->seekSet(0);
			delete message;
		}

		if (blob->getSize())
		{
			socket->writeStream(blob, blob->getSize());
			blob->clear();
		}
	}
}

// ================================================================

UDPSocketsSample::Receiver::Receiver(const Unigine::String &hostname, unsigned short port)
{
	thread.start(hostname, port);

	PlayerPtr player = Game::getPlayer();
	if (player)
		player->setControlled(false);
}

UDPSocketsSample::Receiver::~Receiver()
{
	PlayerPtr player = Game::getPlayer();
	if (player)
		player->setControlled(true);
}

void UDPSocketsSample::Receiver::update()
{
	// Handle incoming messages from the peer.

	if (!thread.isRunning())
		return;

	Message *message = thread.receive();
	int processed = 0;

	while (message && processed < message_process_limit)
	{
		switch (message->getType())
		{
			case Message::TEXT:
			{
				TextMessage *text_msg = dynamic_cast<TextMessage *>(message);

				if (text_msg)
					Log::message("Received a text message from peer: \"%s\".\n", text_msg->text.get());

			} break;

			case Message::CAMERA:
			{
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

bool UDPSocketsSample::Receiver::NetworkThread::start(const String &hostname, unsigned short port)
{
	// Create a UDP socket and bind to the given address.

	socket = Socket::create(Socket::SOCKET_TYPE_DGRAM, hostname, port);

	if (!socket->isOpened())
	{
		Log::warning("Could not resolve specified hostname (%s)!\n", hostname.get());
		return false;
	}

	// Set the socket's receive buffer size.
	socket->recv(RECV_BUFFER_SIZE);

	// Set the socket as non-blocking.
	socket->nonblock();

	// Bind the socket to the address provided the in the `Socket::create` call above.
	if (!socket->bind())
	{
		Log::warning("Could not bind socket to the specified address (%s:%d)!\n", hostname.get(), port);
		return false;
	}

	run();
	return true;
}

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

UDPSocketsSample::Message *UDPSocketsSample::Receiver::NetworkThread::receive()
{
	return queue.pop();
}

void UDPSocketsSample::Receiver::NetworkThread::process()
{
	// Receive incoming messages from the peer, parse and push them to the outgoing messages queue.

	BlobPtr blob = Blob::create();

	while (isRunning())
	{
		socket->readStream(blob, RECV_BUFFER_SIZE);
		blob->seekSet(0);

		if (blob->getSize())
		{
			Message *message = extract_message(blob);

			if (message)
				queue.push(message);
		}

		blob->clear();
	}
}

UDPSocketsSample::Message *UDPSocketsSample::Receiver::NetworkThread::extract_message(Unigine::BlobPtr blob)
{
	Message::Header header = {};
	Message *message = nullptr;

	blob->read(&header, sizeof(header));
	blob->seekSet(0);

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

size_t UDPSocketsSample::TextMessage::pack(Unigine::BlobPtr dst_blob)
{
	size_t cursor = dst_blob->tell();
	size_t packed = 0;

	packed += dst_blob->write(&header, sizeof(header));

	size_t text_size = text.size();
	packed += dst_blob->write(&text_size, sizeof(text_size));
	packed += dst_blob->write(text.get(), text_size);

	header.type = getType();
	header.size = packed;

	dst_blob->seekSet(cursor);
	dst_blob->write(&header, sizeof(header));

	dst_blob->seekSet(cursor + packed);

	return packed;
}

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

size_t UDPSocketsSample::CameraMessage::unpack(Unigine::BlobPtr src_blob)
{
	size_t unpacked = 0;

	unpacked += src_blob->read(&header, sizeof(header));
	unpacked += src_blob->read(&position, sizeof(position));
	unpacked += src_blob->read(&rotation, sizeof(rotation));

	return unpacked;
}

// ================================================================

void UDPSocketsSample::SampleGui::on_start_sender_btn_clicked(const WidgetPtr &widget, int mouse)
{
	if (sample->peer)
	{
		delete sample->peer;
		sample->peer = nullptr;
	}

	start_recver_btn->getEventClicked().setEnabled(false);
	start_sender_btn->getEventClicked().setEnabled(false);

	if (start_sender_btn->isToggled())
	{
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = recv_host_el->getText();
		port = (unsigned short) String::atoi(recv_port_el->getText());

		sample->peer = new Sender(hostname, port);

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
		StringStack<> hostname;
		unsigned short port = 0;

		hostname = recv_host_el->getText();
		port = (unsigned short) String::atoi(recv_port_el->getText());

		sample->peer = new Receiver(hostname, port);

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
}

void UDPSocketsSample::SampleGui::shutdown()
{
	sample_description_window.shutdown();
	Console::setOnscreen(is_onscreen);
	sample = nullptr;
}
