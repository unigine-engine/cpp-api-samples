#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include "TrackSpawner.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
class TracksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TracksSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);


private:
	void init();
	void shutdown();

	Unigine::Vector<TrackSpawner *> track_spawners;


	// GUI
	void init_gui();
	void shutdown_gui();

	void min_distance_edit_line_callback();
	void max_number_edit_line_callback();

	Unigine::WidgetEditLinePtr min_distance_edit_line;
	Unigine::WidgetEditLinePtr max_number_edit_line;

	SampleDescriptionWindow sample_description_window;

};