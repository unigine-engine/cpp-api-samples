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

	PROP_PARAM(Node, weapon_clipping_node);
private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
};

