#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class SimpleMovement;
class SimpleMovement2;
class SimpleMovement3;

class SimpleMovementInfo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovementInfo, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the sample and demonstrates "
						  "how to use a simple move and rotate of a node.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, simple_movement_node);
	PROP_PARAM(Node, simple_movement2_node);
	PROP_PARAM(Node, simple_movement3_node);

private:
	void init();
	void shutdown();

	void init_components();


	SampleDescriptionWindow sample_description_window;

	Unigine::Math::Mat4 simple_movement_start_transform;
	Unigine::Math::Mat4 simple_movement2_start_transform;
	Unigine::Math::Mat4 simple_movement3_start_transform;

	SimpleMovement *simple_movement = nullptr;
	SimpleMovement2 *simple_movement2 = nullptr;
	SimpleMovement3 *simple_movement3 = nullptr;
};
