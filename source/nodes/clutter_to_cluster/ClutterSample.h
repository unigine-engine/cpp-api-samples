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

	PROP_PARAM(Node, clutterConverter);

private:
	ClutterConverter *clutter_converter = nullptr;
	SampleDescriptionWindow sample_description_window;

private:
	void init();
	void shutdown();

	void init_gui();

	void generate_button_callback();
	void convert_button_callback();
};