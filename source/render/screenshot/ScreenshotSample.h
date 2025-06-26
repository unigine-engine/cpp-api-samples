#pragma once

#include "Screenshot.h"

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class ScreenshotSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ScreenshotSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

private:
	Screenshot screenshot;

	SampleDescriptionWindow sample_description_window;
};
