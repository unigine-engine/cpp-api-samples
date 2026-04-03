// Demonstrates terrain height slicing using LandscapeLayerMap. The HeightSlicer
// component copies height data from a source terrain layer and applies a threshold
// cut, modifying a target layer's albedo and height based on the slice level.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "HeightSlicer.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class CombinedSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CombinedSample, Unigine::ComponentBase);
	// Init order 1 ensures this runs after HeightSlicer component
	COMPONENT_INIT(init, 1);
	COMPONENT_SHUTDOWN(shutdown);

	// Node containing the HeightSlicer component
	PROP_PARAM(Node, height_slicer_param, "Height Slicer");

	// Source layer provides height data to read from
	PROP_PARAM(Node, source_lmap_param, "Source Lmap");
	// Target layer receives the sliced result
	PROP_PARAM(Node, target_lmap_param, "Target Lmap");


private:
	void init();
	void shutdown();

	HeightSlicer *height_slicer{nullptr};
	// Normalized slice threshold (0.0 = base, 1.0 = max height)
	float slice_height{1.0f};

	Unigine::LandscapeLayerMapPtr source_lmap;
	Unigine::LandscapeLayerMapPtr target_lmap;

	bool flag{false};

	// UI
	void init_gui();
	void shutdown_gui();

private:
	SampleDescriptionWindow sample_description_window;
	Unigine::WidgetSliderPtr slider;
};
