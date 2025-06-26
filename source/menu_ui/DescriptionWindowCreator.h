#pragma once

#include "SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class DescriptionWindowCreator : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DescriptionWindowCreator, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);
	
	PROP_PARAM(Mask, window_align, 0);
	PROP_PARAM(Mask, window_width, 400);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
};
