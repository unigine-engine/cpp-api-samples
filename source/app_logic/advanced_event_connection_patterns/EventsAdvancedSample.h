#pragma once

#include <UnigineComponentSystem.h>

// Event emitter that broadcasts rotation events based on keyboard input.
// Demonstrates EventInvoker for custom event creation with typed parameters.
// Subscribers can listen to individual axis events or the combined rotate event.
class EventsAdvancedSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EventsAdvancedSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates generating rotation events "
							"on key press for subscription by other components.");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Rotation speed for X, Y, Z axes
	PROP_PARAM(Vec3, rotation_speed, {3.0f, 3.0f, 3.0f});

	// Accessors for rotation events
	Unigine::Event<float> &getEventRotateX() { return rotate_x_event; }
	Unigine::Event<float> &getEventRotateY() { return rotate_y_event; }
	Unigine::Event<float> &getEventRotateZ() { return rotate_z_event; }
	Unigine::Event<float, float, float, EventsAdvancedSample *> &getEventRotate() { return rotate_event; }

protected:
	void init();
	void update();
	void shutdown();

private:
	// Events triggered by input keys
	Unigine::EventInvoker<float> rotate_x_event;
	Unigine::EventInvoker<float> rotate_y_event;
	Unigine::EventInvoker<float> rotate_z_event;
	Unigine::EventInvoker<float, float, float, EventsAdvancedSample *> rotate_event;
};
