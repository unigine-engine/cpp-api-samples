#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class CameraShiftController;

class TwoPointPerspectiveSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TwoPointPerspectiveSample, Unigine::ComponentBase);
	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, controlledPlayer);

private:
	void init();
	void shutdown();

	void init_gui();
	void shutdown_gui();

	void on_checkbox_changed();

private:
	CameraShiftController *camera_controller = nullptr;

	Unigine::WidgetCheckBoxPtr checkbox;
	SampleDescriptionWindow sample_description_window;
};
