#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class SlingRope;

class CraneRopeSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CraneRopeSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow sample_description_window;
	SlingRope *rope_component = nullptr;
};
