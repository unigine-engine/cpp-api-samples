#pragma once

#include <UnigineComponentSystem.h>

class Crosshair
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Crosshair, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, crosshair_image);
	PROP_PARAM(IVec2, crosshair_size, {25, 25});

private:
	void init();
	void shutdown();

private:
	Unigine::WidgetSpritePtr crosshair;
};
