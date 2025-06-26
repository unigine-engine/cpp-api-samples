#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineLights.h>
#include "SunController.h"
#include "DayNightSwitcher.h"

#include "../../menu_ui/SampleDescriptionWindow.h"



class DayNightSwitchSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DayNightSwitchSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the day and night switching "
		"and allows sun rotation to demonstrate how it works.");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	PROP_PARAM(Node, sun_node, "Sun node", "Sun node");
	PROP_PARAM(Node, switcher_node, "Day night switcher node", "Day night switcher node");

	SampleDescriptionWindow window;
	Unigine::WidgetLabelPtr time_label;
	Unigine::WidgetSliderPtr time_slider;

	SunController* sun = nullptr;
	DayNightSwitcher* switcher = nullptr;

	void init();
	void shutdown();
	void update();
	Unigine::String getTimeString(int minutes);
};
