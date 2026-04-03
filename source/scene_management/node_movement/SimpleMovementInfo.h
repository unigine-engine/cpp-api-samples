// Sample UI for the node movement demonstration.
// Provides velocity/angular velocity sliders and reset button
// that apply to all three movement variant components.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class SimpleMovement;
class SimpleMovement2;
class SimpleMovement3;

// UI controller for movement parameter adjustment and position reset.
class SimpleMovementInfo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovementInfo, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the sample and demonstrates "
						  "how to use a simple move and rotate of a node.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// References to nodes with each movement variant
	PROP_PARAM(Node, simple_movement_node);
	PROP_PARAM(Node, simple_movement2_node);
	PROP_PARAM(Node, simple_movement3_node);

private:
	void init();
	void shutdown();
	// Retrieves component references and stores initial transforms
	void init_components();

	SampleDescriptionWindow sample_description_window;

	// Initial transforms saved for reset functionality
	Unigine::Math::Mat4 simple_movement_start_transform;
	Unigine::Math::Mat4 simple_movement2_start_transform;
	Unigine::Math::Mat4 simple_movement3_start_transform;

	// Cached component references for parameter modification
	SimpleMovement *simple_movement = nullptr;
	SimpleMovement2 *simple_movement2 = nullptr;
	SimpleMovement3 *simple_movement3 = nullptr;
};
