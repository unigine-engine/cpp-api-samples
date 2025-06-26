#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>

class BodyFractureExplosionSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureExplosionSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, target);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
};
