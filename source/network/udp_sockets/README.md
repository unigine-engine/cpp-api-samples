# UDP Sockets

This sample shows how to use the sockets API to send and receive UDP messages between two peers in the network.

**You need to have two instances of this 'C++ Samples' app running for this sample to work.**

Each instance can operate in one of two modes: *Sender* or *Receiver*. To select the mode click on the corresponding button below. There you can also specify the *Receiver's hostname and port*.

In *Sender* mode the app packs the player's camera transform into a datagram and sends it to the Receiver on every engine update.

While in this mode you can also send text messages to the peer by using this sample-specific console command `send_msg` (e.g. `**send_msg hello world**`).

In *Receiver* mode the app receives and interprets incoming messages from the peer: the text messages are written to console, and the camera transforms are applied to the player.