// User interface for clutter-to-cluster conversion demo. Provides interactive
// controls for regenerating procedural clutter placement and converting current
// instances to a static cluster for improved runtime performance.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ClutterConverter;

class ClutterSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ClutterSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Node containing the ClutterConverter component to control
	PROP_PARAM(Node, clutterConverter);

private:
	// Cached pointer to the converter component for button callbacks
	ClutterConverter *clutter_converter = nullptr;
	// UI window containing the sample controls
	SampleDescriptionWindow sample_description_window;

private:
	void init();
	void shutdown();

	// Creates the button panel for generate and convert operations
	void init_gui();

	// Triggers clutter seed randomization when Generate button is clicked
	void generate_button_callback();
	// Triggers clutter-to-cluster conversion when Convert button is clicked
	void convert_button_callback();
};
