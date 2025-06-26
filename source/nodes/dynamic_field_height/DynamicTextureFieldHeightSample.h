#pragma once
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class DynamicTextureFieldHeightSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(DynamicTextureFieldHeightSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, field_height, "", "", "", "filter=FieldHeight");

private:
	void init();
	void shutdown();

private:

	SampleDescriptionWindow description_window;
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;
	bool current_mouse_grab_state{ false };
	bool mouse_grab_state_at_init{ false };

	Unigine::WidgetLabelPtr size_map;
};