#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates all four event connection patterns in UNIGINE:
// 1. EventConnection - single callback with enable/disable control
// 2. EventConnections - multiple callbacks with shared lifetime
// 3. Inheriting from EventConnections - automatic cleanup on destruction
// 4. connectUnsafe() with EventConnectionId - manual lifetime management
class EventsSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(EventsSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates different ways to connect and manage UNIGINE events.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)


private:
	// Helper class that owns and fires an event for demonstration
	class EventHolder
	{
	public:
		Unigine::Event<int> &getEvent() { return event; } // Returns event for subscription
		void run(int value); // Fires event with given value

	private:
		Unigine::EventInvoker<int> event; // Event that can be invoked and subscribed to
	};

	// Example of inheriting from EventConnections to manage callbacks automatically.
	// All connected callbacks are disconnected when this object is destroyed.
	class InheritedEventConnectionExample final : public EventConnections
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventInvoker<int> event;
	};

	// Example of a single EventConnection
	// allowing enabling/disabling the callback manually.
	class EventConnectionExample
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventConnection connection;
	};


	// Example of managing multiple callbacks with EventConnections.
	// Callbacks are automatically disconnected when the object is destroyed or disconnectAll() is called.
	class EventConnectionsExample
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventConnections connections;
	};


	// Example of unsafe connection, where you must manage the callback lifetime yourself.
	// Use this only if you know exactly what you're doing.
	class CallbackIDConnection
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe(Unigine::Event<int> &event);

	private:
		Unigine::EventConnectionId callback_id{};
	};

private:
	void init();
	void shutdown();

private:
	EventHolder holder; // Event source that all examples subscribe to

	// One instance of each connection pattern for demonstration
	InheritedEventConnectionExample inherited_event_connection_example;
	EventConnectionExample event_connection_example;
	EventConnectionsExample event_connections_example;
	CallbackIDConnection callback_id_connection;
};
