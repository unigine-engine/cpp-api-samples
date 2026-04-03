// Sample controller for object frame screenshot capture functionality.
// Captures framebuffer and saves PNG screenshot with JSON metadata file.
// Temporarily hides frames during capture to get clean screenshot.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ObjectFrame;

// Captures screenshots with frame metadata for object detection datasets.
class ObjectFrameSample final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ObjectFrameSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	void snap_screenshot();

	SampleDescriptionWindow sample_description_window;
	// Main window for frame capture
	Unigine::EngineWindowPtr window;
	// Status label showing last saved filename
	Unigine::WidgetLabelPtr label;

	// All ObjectFrame components in the scene
	Unigine::Vector<ObjectFrame *> frames;
	// Flag set when screenshot capture is pending
	bool grab_flag{false};
	// Timestamp buffer for filenames
	Unigine::String time_str{"yyyy-mm-dd hh_mm_ss"};
};
