#pragma once

#include <UnigineComponentSystem.h>

// Event subscriber demonstrating various connection patterns.
// Shows member method binding, lambda with captured node reference,
// EventConnection for enable/disable control, and EventConnectionId for manual management.
class EventsAdvancedUnit
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EventsAdvancedUnit, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates advanced event subscriptions "
						"using methods, lambdas, and connection IDs.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, input_manager); // Node with EventsAdvancedSample to subscribe to

private:
	void init();
	void shutdown();

	// Event handlers for different rotation axes
	void rotate(float x, float y, float z);                                   // Member method handler
	static void rotateNode(float x, float y, float z, Unigine::NodePtr node); // Static handler for XYZ
	static void rotateNodeY(float angle, Unigine::NodePtr node);              // Static handler for Y axis

private:
	// Connection objects for different subscription patterns
	Unigine::EventConnection rotate_y_connection;     // For enable/disable control
	Unigine::EventConnectionId rotate_z_connection_id; // For manual disconnect by ID
	Unigine::EventConnectionId rotate_connection_id;   // For manual disconnect by ID
};
