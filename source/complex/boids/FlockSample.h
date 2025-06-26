#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>


class FlockController;
class FlockSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(FlockSample, Unigine::ComponentBase)
	PROP_PARAM(Node, bird_controller_parameter)
	PROP_PARAM(Node, fish_controller_parameter)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void init_ui();
	void update();
	void shutdown();

	void visualize_flock();

private:
	SampleDescriptionWindow sample_description_window;
	FlockController *fish_controller;
	FlockController *bird_controller;

	bool is_visualizer_enabled_at_sample_start{false};

	bool is_debug_enabled{false};
};
