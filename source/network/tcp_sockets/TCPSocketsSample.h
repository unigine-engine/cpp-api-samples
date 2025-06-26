#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include <UnigineStreams.h>
#include <UnigineThread.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class TCPSocketsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TCPSocketsSample, Unigine::ComponentBase);

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	// A base class for messages to exchange between peers.
	struct Message
	{
		enum TYPE { TEXT, CAMERA, NUM_TYPES };

		struct Header
		{
			int type = TEXT;
			int size = 0; // The message size (including header).
		};

		virtual ~Message() = default;

		virtual int getType() const = 0;
		virtual Message *copy() const = 0;
		virtual size_t pack(Unigine::BlobPtr dst_blob) = 0;
		virtual size_t unpack(Unigine::BlobPtr src_blob) = 0;

		Header header;
	};

	// A simple text message.
	struct TextMessage : public Message
	{
		TextMessage() = default;
		TextMessage(TextMessage const &) = default;
		~TextMessage() override = default;

		TextMessage(const Unigine::StringStack<> &text): text { text } {}

		int getType() const override { return TEXT; }
		virtual Message *copy() const override { return new TextMessage(*this); }
		virtual size_t pack(Unigine::BlobPtr dst_blob) override;
		virtual size_t unpack(Unigine::BlobPtr src_blob) override;

		Unigine::StringStack<> text;
	};

	// A message containing camera's transform.
	struct CameraMessage : public Message
	{
		CameraMessage() = default;
		~CameraMessage() override = default;

		CameraMessage(const Unigine::Math::Vec3 &position, const Unigine::Math::quat &rotation):
			position { position },
			rotation { rotation }
		{}

		int getType() const override { return CAMERA; }
		virtual Message *copy() const override { return new CameraMessage(*this); }
		virtual size_t pack(Unigine::BlobPtr dst_blob) override;
		virtual size_t unpack(Unigine::BlobPtr src_blob) override;

		Unigine::Math::Vec3 position;
		Unigine::Math::quat rotation;
	};

	// ================================================================

	// A thread-safe queue for network messages.
	class MessageQueue
	{
	public:
		void push(Message *message)
		{
			Unigine::ScopedLock lock(mutex);

			queue.push_back(message);

			if (queue.size() >= max_size)
			{
				Message *message = queue.takeFirst();

				if (message)
					delete message;
			}
		}

		Message *pop()
		{
			Unigine::ScopedLock lock(mutex);
			Message *message = nullptr;

			if (queue.size() && (message = queue.takeFirst()))
			{
				Message *original = message;
				message = original->copy();
				delete original;
			}

			return message;
		}

		void clear()
		{
			while (queue.size())
			{
				Message *message = queue.takeLast();
				if (message)
					delete message;
			}
		}

	private:
		Unigine::Vector<Message *> queue;
		Unigine::Mutex mutex;
		int max_size = 8;
	};

	// ================================================================

	// A simple abstract class for Client and Server, a helper to make this sample's logic more concise.
	struct IHost
	{
		virtual void update() = 0;
		virtual ~IHost() = default;
	};

	// The Client, which can send and receive messages to and from the Server.
	class Client : public IHost
	{
	public:

		// The Client network thread. Handles outgoing and incoming messages from/to the Server.
		class NetworkThread : public Unigine::Thread
		{
		public:
			NetworkThread() = default;
			NetworkThread(Unigine::SocketPtr socket, bool connect) { start(socket, connect); }
			~NetworkThread() { reset(); }

			void start(Unigine::SocketPtr socket, bool connect); // Start the network thread using the provided socket. Optionally "connect" to the socket.
			void send(Message *message); // Put a message to the outgoing messages queue. It will be sent later in the thread's `process` method.
			Message *receive(); // Retrieve a message from the incoming messages queue. If the queue is empty, null pointer is returned.

			bool isConnectionActive() const { return isRunning(); }
			char const *getHostname() const { return socket ? socket->getHost() : ""; }
			unsigned short getPort() const { return socket ? socket->getPort() : 0; }

		private:
			void process() override; // The thread's process method containg all of the network message handling logic.
			void reset(); // Stop the the thread and clear its state.

			Message *extract_message(Unigine::BlobPtr blob); // Extract a message from the raw binary blob.

			MessageQueue recv_queue;
			MessageQueue send_queue;
			Unigine::SocketPtr socket;
			bool connect = false;
		};

		Client(const Unigine::String &hostname, unsigned short port);
		~Client() override;

		bool isConnectionActive() const { return thread.isConnectionActive(); }

		char const *getHostname() const { return thread.getHostname(); }
		unsigned short getPort() const { return thread.getPort(); }

		void update() override; // Client's update function. Here incoming messages from the Server are handled and new messages are appended the outgoing message queue.

	private:
		void on_message_send_cmd(int argc, char **argv);

		NetworkThread thread;
		int message_process_limit = 8;
	};

	// ================================================================

	// The Server. It manages multiple Client connections, and handles incoming & outgoing messages from/to the clients.
	class Server : public IHost
	{
	public:
		Server(const Unigine::String &hostname, unsigned short port);
		~Server() override;

		void update() override; // The Server's update function. Here client connections are updated and incoming/outgoing messages are handled.

		const Unigine::Vector<Client::NetworkThread *> &getClients() const { return clients; }

	private:

		// The Server's network thread. It simply accepts new Client connections.
		class NetworkThread : public Unigine::Thread
		{
		public:
			~NetworkThread() { reset(); }

			void start(Unigine::SocketPtr socket);
			Unigine::SocketPtr accept(); // Take a new accepted client connection as a socket.

		private:
			void process() override; // The thread's process method containg all of the network handling logic.
			void reset();

			Unigine::Vector<Unigine::SocketPtr> connections; // Accepted connections queue.
			Unigine::SocketPtr socket; // The Server's listen socket.
			Unigine::Mutex mutex;
		};

		void on_message_send_cmd(int argc, char **argv);

		Unigine::Vector<Client::NetworkThread *> clients; // Client network threads for each of the accepted client connections.
		NetworkThread thread;
	};

	// ================================================================

	struct SampleGui : Unigine::EventConnections
	{
		void init(TCPSocketsSample *sample);
		void update();
		void shutdown();

		void on_start_server_btn_clicked(const Unigine::WidgetPtr &widget, int mouse);
		void on_start_client_btn_clicked(const Unigine::WidgetPtr &widget, int mouse);

		bool is_onscreen = false;
		TCPSocketsSample *sample = nullptr;
		Unigine::WidgetButtonPtr start_client_btn;
		Unigine::WidgetButtonPtr start_server_btn;
		Unigine::WidgetEditLinePtr server_port_el;
		Unigine::WidgetEditLinePtr server_hostname_el;
		Unigine::WidgetGroupBoxPtr client_connections_gbox;
		SampleDescriptionWindow sample_description_window;
	};

	void init();
	void update();
	void shutdown();

	IHost *host = nullptr;
	SampleGui gui;
};
