#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class BoatSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BoatSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, water, "Water", "", "", "filter=ObjectWaterGlobal");
	PROP_PARAM(Float, particles_life_time_factor, 1.0f);

private:
	void init();
	void shutdown();
	SampleDescriptionWindow sample_description_window;
};