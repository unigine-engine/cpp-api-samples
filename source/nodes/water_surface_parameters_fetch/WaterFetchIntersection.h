// Demonstrates two methods for querying water surface state: fetch (direct height
// lookup) and intersection (ray-water collision). Fetch is faster but less accurate
// for angled queries; intersection handles oblique rays with configurable precision.

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

	// Reference to the ObjectWaterGlobal node to query
	PROP_PARAM(Node, water_node, "", "", "", "filter=ObjectWaterGlobal");

private:
	void init();
	void update();
	void shutdown();

	void init_gui();
	void shutdown_gui();
	Unigine::WidgetWindowPtr window;

	// Number of query points to visualize (sqrt gives grid dimensions)
	int num_intersection = 100;
	// Size of rendered intersection points
	float intersect_point_size = 0.2f;
	// Ray angle for intersection queries (degrees from vertical)
	float intersection_angle = 0.f;
	// Mode switch: true = fetch (height lookup), false = intersection (ray cast)
	bool fetch = true;
	SampleDescriptionWindow sample_description_window;
	// Checkbox to toggle normal vector visualization
	Unigine::WidgetCheckBoxPtr normal_cb;
	// Cached pointer to the water object
	Unigine::ObjectWaterGlobalPtr water;
};
