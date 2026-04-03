#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class PhysicsMover;

// Provides UI controls for adjusting physics movement parameters in real-time.
// Sliders allow tuning force, torque, speed limits, and braking strength.
class PhysicsMovementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicsMovementSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, car); // Vehicle node with PhysicsMover component

private:
	void init();         // Retrieves mover component and creates UI
	void shutdown();     // Closes UI window

	void init_gui();     // Creates parameter sliders
	void shutdown_gui(); // Releases UI resources

private:
	Unigine::NodePtr car_node;             // Cached reference to car node

	PhysicsMover* physics_mover = nullptr; // Movement controller to configure

	SampleDescriptionWindow sample_description_window; // Parameter UI window
};
