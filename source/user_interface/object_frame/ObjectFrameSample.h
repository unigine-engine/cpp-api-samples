#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ObjectFrame;

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
	Unigine::EngineWindowPtr window;
	Unigine::WidgetLabelPtr label;

	Unigine::Vector<ObjectFrame *> frames;
	bool grab_flag{false};
	Unigine::String time_str{"yyyy-mm-dd hh_mm_ss"};
};
