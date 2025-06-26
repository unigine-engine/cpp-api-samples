#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>

class MicroprofilerSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MicroprofilerSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
	Unigine::Engine::BACKGROUND_UPDATE previous_bg_update;
};
