// Sample UI for the screenshot capture functionality.
// Provides a button to trigger screenshot capture via
// the Screenshot helper class.

#pragma once

#include "Screenshot.h"

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Provides UI button to trigger screenshot capture and save to file.
class ScreenshotSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ScreenshotSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

private:
	// Helper object that handles actual screenshot capture
	Screenshot screenshot;

	SampleDescriptionWindow sample_description_window;
};
