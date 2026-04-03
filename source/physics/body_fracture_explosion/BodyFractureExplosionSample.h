// Sample UI for triggering the explosion effect. Provides a button that
// retrieves the BodyFractureExplosion component from the target node and
// initiates the explosion when clicked.

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

	// Node containing the BodyFractureExplosion component to trigger
	PROP_PARAM(Node, target);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
};
