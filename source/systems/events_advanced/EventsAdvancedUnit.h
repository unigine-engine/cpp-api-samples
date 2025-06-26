#pragma once

#include <UnigineComponentSystem.h>

class EventsAdvancedUnit
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EventsAdvancedUnit, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, input_manager);

private:
	void init();
	void shutdown();

	void rotate(float x, float y, float z);
	static void rotateNode(float x, float y, float z, Unigine::NodePtr node);
	static void rotateNodeY(float angle, Unigine::NodePtr node);

private:
	Unigine::EventConnection rotate_y_connection;
	Unigine::EventConnectionId rotate_z_connection_id;
	Unigine::EventConnectionId rotate_connection_id;
};
