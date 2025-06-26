#pragma once

#include <UnigineComponentSystem.h>

class EventsAdvancedSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EventsAdvancedSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec3, rotation_speed, {3.0f, 3.0f, 3.0f});

	Unigine::Event<float> &getEventRotateX() { return rotate_x_event; }
	Unigine::Event<float> &getEventRotateY() { return rotate_y_event; }
	Unigine::Event<float> &getEventRotateZ() { return rotate_z_event; }
	Unigine::Event<float, float, float, EventsAdvancedSample *> &getEventRotate() { return rotate_event; }

protected:
	void init();
	void update();
	void shutdown();

private:
	Unigine::EventInvoker<float> rotate_x_event;
	Unigine::EventInvoker<float> rotate_y_event;
	Unigine::EventInvoker<float> rotate_z_event;
	Unigine::EventInvoker<float, float, float, EventsAdvancedSample *> rotate_event;
};
