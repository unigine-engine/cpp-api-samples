#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
class BuoySample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BuoySample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, buoyancy, 1.0f);
	PROP_PARAM(Node, water, "Water", "", "", "filter=ObjectWaterGlobal");

private:
	void init();
	void shutdown();

	SampleDescriptionWindow sample_description_window;
};
