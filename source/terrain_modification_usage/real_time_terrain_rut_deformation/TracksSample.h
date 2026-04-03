// Demonstrates terrain track spawning with configurable parameters.
// Provides GUI controls for adjusting track spacing and pool size.
// Manages multiple TrackSpawner components in child hierarchy.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include "TrackSpawner.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Controller for track spawning sample with parameter GUI.
class TracksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TracksSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	Unigine::Vector<TrackSpawner *> track_spawners;		// All spawner components in children

	// GUI initialization and cleanup
	void init_gui();
	void shutdown_gui();

	void min_distance_edit_line_callback();		// Handles distance parameter change
	void max_number_edit_line_callback();		// Handles pool size parameter change

	Unigine::WidgetEditLinePtr min_distance_edit_line;	// Distance input widget
	Unigine::WidgetEditLinePtr max_number_edit_line;	// Pool size input widget

	SampleDescriptionWindow sample_description_window;	// Reusable window component
};