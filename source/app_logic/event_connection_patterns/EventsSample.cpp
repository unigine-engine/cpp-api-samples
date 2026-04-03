// Demonstrates four patterns for connecting to Unigine events:
// 1) EventConnection - single callback with enable/disable control
// 2) EventConnections - multiple callbacks managed as a group
// 3) Inherited EventConnections - class inherits connection management
// 4) Callback ID - manual management via connectUnsafe() and disconnect(id)

#include "EventsSample.h"

#include <UnigineConsole.h>

using namespace Unigine;

void EventsSample::EventHolder::run(int value)
{
	event.run(value);
}

void EventsSample::InheritedEventConnectionExample::subscribe(Unigine::Event<int> &event)
{
	// Since this class inherits from Unigine::EventConnections,
	// we can pass *this to connect(). The object will manage the connection,
	// automatically disconnecting callbacks when destroyed.
	// This is similar to EventConnectionsExample.
	event.connect(*this,
		[](int value) { Log::message("InheritedEventConnectionExample event called\n", value); });
}

void EventsSample::InheritedEventConnectionExample::unsubscribe()
{
	// Disconnect all events associated with this object
	disconnectAll();
}

void EventsSample::EventConnectionExample::subscribe(Unigine::Event<int> &event)
{
	// Pass the connection object to store the callback.
	// Useful when you need precise control over a specific callback.
	event.connect(connection,
		[](int value) { Log::message("EventConnectionExample event called\n", value); });

	// Enable or disable the callback through the connection
	connection.setEnabled(true);
}

void EventsSample::EventConnectionExample::unsubscribe()
{
	// Disconnect the callback
	connection.disconnect();
}

void EventsSample::EventConnectionsExample::subscribe(Unigine::Event<int> &event)
{
	// Pass an Unigine::EventConnections object to store multiple callbacks.
	// Convenient for managing many callbacks at once, but cannot enable/disable individually.
	// Useful when multiple callbacks share the same lifetime.
	event.connect(connections,
		[](int value) { Log::message("EventConnectionsExample event called\n", value); });
}

void EventsSample::EventConnectionsExample::unsubscribe() 
{
	// No explicit disconnect needed; connections are automatically cleared on object destruction
}

void EventsSample::CallbackIDConnection::subscribe(Unigine::Event<int> &event)
{
	// connectUnsafe returns an ID representing the callback, 
	// allowing manual management (disconnecting or tracking) of this specific callback
	callback_id = event.connectUnsafe(
		[](int value) { Log::message("CallbackIDConnection event called\n", value); });
}

void EventsSample::CallbackIDConnection::unsubscribe(Unigine::Event<int> &event)
{
	// Store the event to later disconnect the callback using its ID
	// You manage the callback lifetime manually in this approach
	event.disconnect(callback_id);
}


REGISTER_COMPONENT(EventsSample)

// All four connection patterns are subscribed and the event is fired once.
void EventsSample::init()
{
	event_connection_example.subscribe(holder.getEvent());
	event_connections_example.subscribe(holder.getEvent());
	inherited_event_connection_example.subscribe(holder.getEvent());
	callback_id_connection.subscribe(holder.getEvent());

	holder.run(42);
	Console::setOnscreen(true);
}

// All connection patterns are unsubscribed to demonstrate cleanup.
void EventsSample::shutdown()
{
	event_connection_example.unsubscribe();
	event_connections_example.unsubscribe();
	inherited_event_connection_example.unsubscribe();
	callback_id_connection.unsubscribe(holder.getEvent());

	Console::setOnscreen(false);
}
