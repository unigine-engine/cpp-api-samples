// Sample shell for the distance-measurement components. It enables the
// Visualizer (which draws everything), collects the DistanceMeasurement and
// ZoneRadiusVisualizer components in the world and exposes two sliders that
// drive them all at once. The measuring and drawing live in those components,
// so each can be reused on its own node without this sample.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineVector.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "DistanceMeasurement.h"
#include "ZoneRadiusVisualizer.h"

class MeasurementsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MeasurementsSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("Dynamically measures and visualizes the distance from a movable node to several target "
		"nodes using the Visualizer.");

	COMPONENT_INIT(init,10);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	SampleDescriptionWindow description_window;

	// All measurement/zone components in the world, driven together by the two
	// sliders below.
	Unigine::Vector<DistanceMeasurement *> measurements;
	Unigine::Vector<ZoneRadiusVisualizer *> zones;
};
