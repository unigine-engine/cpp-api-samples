#pragma once

#include <UnigineComponentSystem.h>

class FFPSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FFPSample, ComponentBase)
	COMPONENT_INIT(init)

private:
	void init();
	static void render();
};
