// Sample UI demonstrating camera zoom with real-time display of FOV,
// mouse sensitivity, and render distance scale adjustments.
// Provides target buttons to quickly orient camera toward objects.

#pragma once

#include "ZoomController.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// GUI panel that controls zoom level and displays camera parameters.
class ZoomSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ZoomSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the zoom sample and demonstrates "
		"how to use a simple zoom and how it affects player camera.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, zoom_node, "", "Zoom controller node");		// Player with ZoomController attached
	PROP_PARAM(Node, target_one_node, "", "Target one node");	// First focus target
	PROP_PARAM(Node, target_two_node, "", "Target two node");	// Second focus target
	PROP_PARAM(Node, target_three_node, "", "Target three node");	// Third focus target

private:
	Unigine::Input::MOUSE_HANDLE mouse_grab = Unigine::Input::MOUSE_HANDLE_USER;

	void init();
	void shutdown();
	void init_components();

	SampleDescriptionWindow window_sample;

	Unigine::WidgetLabelPtr fov_label;				// Shows current FOV in degrees
	Unigine::WidgetLabelPtr mouse_sensivity_label;	// Shows adjusted mouse sensitivity
	Unigine::WidgetLabelPtr render_scale_label;		// Shows LOD distance multiplier

	ZoomController* zoom = nullptr;
	Unigine::PlayerPtr player;
};
