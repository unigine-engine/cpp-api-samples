#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>

// Demonstrates Microprofiler integration for detailed performance analysis.
// Displays the microprofiler URL for browser-based profiling when available.
class MicroprofilerSample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MicroprofilerSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();     // Displays microprofiler URL and enables background updates
	void shutdown(); // Restores background update settings

private:
	// Sample UI with description and controls
	SampleDescriptionWindow sample_description_window;
	Unigine::Engine::BACKGROUND_UPDATE previous_bg_update;
};
