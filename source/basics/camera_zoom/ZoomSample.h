#pragma once

#include "ZoomController.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ZoomSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ZoomSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the zoom sample and demonstrates "
		"how to use a simple zoom and how it affects player camera.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, zoom_node, "", "Zoom controller node");

	PROP_PARAM(Node, target_one_node, "", "Target one node");
	PROP_PARAM(Node, target_two_node, "", "Target two node");
	PROP_PARAM(Node, target_three_node, "", "Target three node");

private:
	Unigine::Input::MOUSE_HANDLE mouse_grab = Unigine::Input::MOUSE_HANDLE_USER;
	void init();
	void shutdown();

	void init_components();

	SampleDescriptionWindow window_sample;

	Unigine::WidgetLabelPtr fov_label;
	Unigine::WidgetLabelPtr mouse_sensivity_label;
	Unigine::WidgetLabelPtr render_scale_label;

	ZoomController* zoom = nullptr;
	Unigine::PlayerPtr player;
};
