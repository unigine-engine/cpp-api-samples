// Sample shell: enables the Visualizer (which draws every ruler and zone) and
// exposes two sliders that drive all DistanceMeasurement / ZoneRadiusVisualizer
// components in the world at once.

#include "MeasurementsSample.h"

#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Unigine::Math;

REGISTER_COMPONENT(MeasurementsSample);

void MeasurementsSample::init()
{
	// The Visualizer is the only thing drawing the result, so enable it here and
	// disable it again in shutdown().
	Visualizer::setEnabled(true);

	// The measuring and drawing live in these components; the sample only drives
	// their shared parameters from one place.
	ComponentSystem::get()->getComponentsInWorld(measurements, true);
	ComponentSystem::get()->getComponentsInWorld(zones, true);

	description_window.createWindow();

	// One slider drives every DistanceMeasurement at once.
	float green_distance = measurements.size() ? measurements[0]->green_distance.get() : 25.0f;
	description_window.addFloatParameter("Green distance", "Distance at which a ruler turns fully green",
		green_distance, 1.0f, 100.0f, [this](float value) {
			for (int i = 0; i < measurements.size(); i++)
				if (measurements[i])
					measurements[i]->green_distance = value;
		});

	// One slider drives every ZoneRadiusVisualizer at once.
	float zone_radius = zones.size() ? zones[0]->zone_radius.get() : 3.0f;
	description_window.addFloatParameter("Zone radius", "Radius drawn around each target node",
		zone_radius, 0.0f, 20.0f, [this](float value) {
			for (int i = 0; i < zones.size(); i++)
				if (zones[i])
					zones[i]->zone_radius = value;
		});
}

void MeasurementsSample::shutdown()
{
	Visualizer::setEnabled(false);
	description_window.shutdown();
}
