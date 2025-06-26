#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "Waves.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class BuoyancyPhysicsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BuoyancyPhysicsSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, waves_node);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
	Waves *waves = nullptr;
};
