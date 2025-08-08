# TCP Sockets

This sample demonstrates how to establish and manage *TCP* socket connections between a server and multiple clients each represented by a UNIGINE-application. Clients can connect to the server, exchange text messages via the Console (**send_msg** command), and receive camera transform updates from the server.

**You need to have two instances of this 'C++ Samples' app running for this sample to work.**

Each instance can operate in one of two modes: *Server* or *Client*. To select the mode click on the corresponding button below. There you can also specify the desired *host and port*.

The server uses a non-blocking socket to accept client connections and creates a dedicated background thread for each connection. The communication protocol is based on custom messages (e.g., text or camera transforms) packed and unpacked using *Blob* streams. On the client side, a socket is created and connected to the server. Incoming and outgoing messages are sent/received using two threadsafe queues. To send text messages to the peer use the sample-specific console command `send_msg` (e.g. `**send_msg hello world**`)

Incoming messages are parsed using message headers. Both client and server use message buffering, timeouts, and validation checks to maintain connection stability and prevent invalid data processing.

The sample provides options to configure the server address and port, switch between modes, and monitor active connections.