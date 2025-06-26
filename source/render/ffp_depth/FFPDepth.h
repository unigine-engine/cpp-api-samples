#pragma once

#include <UnigineComponentSystem.h>

class FFPDepth : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FFPDepth, ComponentBase)
	COMPONENT_INIT(init)

private:
	void init();

	void render_lines_callback();
};
