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
	COMPONENT_INIT(init, 1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, height_slicer_param, "Height Slicer");

	PROP_PARAM(Node, source_lmap_param, "Source Lmap");
	PROP_PARAM(Node, target_lmap_param, "Target Lmap");


private:
	void init();
	void shutdown();

	HeightSlicer *height_slicer{nullptr};
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