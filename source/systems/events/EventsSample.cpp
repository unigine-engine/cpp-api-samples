#include "EventsSample.h"

#include <UnigineConsole.h>

using namespace Unigine;

void EventsSample::EventHolder::run(int value)
{
	event.run(value);
}

void EventsSample::InheritedEventConnectionExample::subscribe(Unigine::Event<int> &event)
{
	// because "this" is inherited from Unigine::EventConnections
	// we can pass to connect *this, and connect will take "this" as a connection
	// this approach is similar to EventConnectionsExample, and you can use your class
	// as event connection, which makes connection disconnect in class destructor
	event.connect(*this,
		[](int value) { Log::message("InheritedEventConnectionExample event called\n", value); });
}

void EventsSample::InheritedEventConnectionExample::unsubscribe()
{
	disconnectAll();
}

void EventsSample::EventConnectionExample::subscribe(Unigine::Event<int> &event)
{
	// here we pass the connection, in which we want to store our callback
	// this connection type is useful when you want a precise control over your callback
	event.connect(connection,
		[](int value) { Log::message("EventConnectionExample event called\n", value); });

	// here you can enable or disable your callback in the connection
	connection.setEnabled(true);
}

void EventsSample::EventConnectionExample::unsubscribe()
{
	connection.disconnect();
}

void EventsSample::EventConnectionsExample::subscribe(Unigine::Event<int> &event)
{
	// here we pass Unigine::EventConnections object to connect, in which we store events
	// you can easily manage lots of callbacks with different connections, but you can't
	// disable or enable connections.
	// it is useful when you have a lot of callbacks with same lifetimes
	event.connect(connections,
		[](int value) { Log::message("EventConnectionsExample event called\n", value); });
}

void EventsSample::EventConnectionsExample::unsubscribe() {}

void EventsSample::CallbackIDConnection::subscribe(Unigine::Event<int> &event)
{
	callback_id = event.connectUnsafe(
		[](int value) { Log::message("CallbackIDConnection event called\n", value); });
}

void EventsSample::CallbackIDConnection::unsubscribe(Unigine::Event<int> &event)
{
	// we need to store event because in event our callback is stored
	// you need to manage lifetimes yourself
	event.disconnect(callback_id);
}


REGISTER_COMPONENT(EventsSample)

void EventsSample::init()
{
	event_connection_example.subscribe(holder.getEvent());
	event_connections_example.subscribe(holder.getEvent());
	inherited_event_connection_example.subscribe(holder.getEvent());
	callback_id_connection.subscribe(holder.getEvent());

	holder.run(42);
	Console::setOnscreen(true);
}

void EventsSample::shutdown()
{
	event_connection_example.unsubscribe();
	event_connections_example.unsubscribe();
	inherited_event_connection_example.unsubscribe();
	callback_id_connection.unsubscribe(holder.getEvent());

	Console::setOnscreen(false);
}
