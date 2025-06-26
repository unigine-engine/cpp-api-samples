#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

/// In Unigine there some variations of how you can manage events
/// In this sample all different event connection types will be showcased

class EventsSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(EventsSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)


private:
	class EventHolder
	{
	public:
		Unigine::Event<int> &getEvent() { return event; }
		void run(int value);

	private:
		Unigine::EventInvoker<int> event;
	};

	// With this approach, all callbacks connected to this event will be disconnected
	// as soon as this class destructor is called
	class InheritedEventConnectionExample final : public EventConnections
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventInvoker<int> event;
	};

	// With this approach, we can only connect single callback to this "connection"
	// doing so we can also enable and disable this callback
	class EventConnectionExample
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventConnection connection;
	};


	// With this approach, all callbacks, which are connected to "connections"
	// will be disconnected when "connections" destructor are called or
	// when we explicitly call disconnectAll function
	class EventConnectionsExample
	{
	public:
		void subscribe(Unigine::Event<int> &event);
		void unsubscribe();

	private:
		Unigine::EventConnections connections;
	};


	// With this approach, we can connect to event unsafely,
	// and by doing so we can manage connection lifetime ourselves
	// in a cost of possible lifetime errors
	// use this option only if you exactly know what you want
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
	// mock object that can provide us with event
	EventHolder holder;

	InheritedEventConnectionExample inherited_event_connection_example;
	EventConnectionExample event_connection_example;
	EventConnectionsExample event_connections_example;
	CallbackIDConnection callback_id_connection;
};
