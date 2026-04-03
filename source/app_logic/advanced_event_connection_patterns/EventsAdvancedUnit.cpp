// Subscriber component that connects to events from EventsAdvancedSample. Shows
// various connection patterns: member method with extra args, static function with
// EventConnection, lambda with connectUnsafe(), and argument discarding.

#include "EventsAdvancedUnit.h"
#include "EventsAdvancedSample.h"

REGISTER_COMPONENT(EventsAdvancedUnit);

using namespace Unigine;

// Four different connection patterns are demonstrated: member method, static function
// with EventConnection, lambda with connectUnsafe, and argument discarding.
void EventsAdvancedUnit::init()
{
	auto inputManager = getComponent<EventsAdvancedSample>(input_manager);

	if (!inputManager)
		return;

	// Connect to class method with extra parameters.
	// It works within component because Unigine::ComponentBase inherits from EventConnection
	inputManager->getEventRotateX().connect(this, &EventsAdvancedUnit::rotate, 0.0f, 0.0f);

	// Connect to function and EventConnection with extra parameter
	inputManager->getEventRotateY().connect(rotate_y_connection, rotateNodeY, node);

	// Connect to lambda and store EventConnectionId to disconnect it later
	rotate_z_connection_id = inputManager->getEventRotateZ().connectUnsafe([this](float angle) {
		Log::message("Rotate Z   (0.0 0.0 %.1f)!\n", angle);
		node->rotate(0, 0, angle);
	});

	// Connect with discarding argument to function and store EventConnectionId to disconnect it later
	rotate_connection_id = inputManager->getEventRotate().connectUnsafe(rotateNode, node);
}

// All four connection patterns are disconnected using their respective methods.
void EventsAdvancedUnit::shutdown()
{
	auto inputManager = getComponent<EventsAdvancedSample>(input_manager);

	if (!inputManager)
		return;

	// Disconnect class method
	// It works within component because Unigine::ComponentBase inherits from EventConnection
	inputManager->getEventRotateX().disconnect(this, &EventsAdvancedUnit::rotate);

	// Disconnect EventConnection
	rotate_y_connection.disconnect();

	// Disconnect by EventConnectionId
	inputManager->getEventRotateZ().disconnect(rotate_z_connection_id);

	// Disconnect by EventConnectionId
	inputManager->getEventRotate().disconnect(rotate_connection_id);
}

// Member method handler: receives X angle from event, Y and Z are extra args (0.0).
void EventsAdvancedUnit::rotate(float x, float y, float z)
{
	Log::message("Rotate X   (%.1f %.1f %.1f)!\n", x, y, z);
	node->rotate(x, y, z);
}

// Static handler for combined XYZ event. Node is passed as extra argument.
void EventsAdvancedUnit::rotateNode(float x, float y, float z, Unigine::NodePtr node)
{
	Log::message("Rotate XYZ (%.1f %.1f %.1f)!\n", x, y, z);
	if (node)
		node->rotate(x, y, z);
}

// Static handler for Y axis rotation. Node is passed as extra argument.
void EventsAdvancedUnit::rotateNodeY(float angle, Unigine::NodePtr node)
{
	Log::message("Rotate Y   (0.0 %.1f 0.0)!\n", angle);
	if (node)
		node->rotate(0, angle, 0);
}
