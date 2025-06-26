#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "PhysicsMovement.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>


class PhysicsMovementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicsMovementSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, car);

private:
	void init();
	void shutdown();

	void init_gui();
	void shutdown_gui();

	void switch_component(bool force);

	bool force = true;

	float max_speed = 30.0f;
	float acceleration = 10.0f;
	float turn_speed = 1.0f;

	SampleDescriptionWindow sample_description_window;

	Unigine::NodePtr car_node;
	PhysicsForceMovement *comp_force = nullptr;
	PhysicsImpulseMovement *comp_impulse = nullptr;

	Unigine::EventConnections button_connections;
};
