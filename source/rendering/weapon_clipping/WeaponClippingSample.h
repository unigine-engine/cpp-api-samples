// Sample UI for weapon clipping prevention demonstration.
// Provides controls to toggle the weapon rendering mode that
// prevents first-person weapons from clipping through walls.

#pragma once
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class WeaponClippingSample :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WeaponClippingSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Node containing the WeaponClipping component
	PROP_PARAM(Node, weapon_clipping_node);
private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
};

