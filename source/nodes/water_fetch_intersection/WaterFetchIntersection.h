#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class WaterFetchIntersection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WaterFetchIntersection, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, water_node, "", "", "", "filter=ObjectWaterGlobal");

private:
	void init();
	void update();
	void shutdown();

	void init_gui();
	void shutdown_gui();
	Unigine::WidgetWindowPtr window;

	int num_intersection = 100;
	float intersect_point_size = 0.2f;
	float intersection_angle = 0.f;
	bool fetch = true;
	SampleDescriptionWindow sample_description_window;
	Unigine::WidgetCheckBoxPtr normal_cb;
	Unigine::ObjectWaterGlobalPtr water;
};
