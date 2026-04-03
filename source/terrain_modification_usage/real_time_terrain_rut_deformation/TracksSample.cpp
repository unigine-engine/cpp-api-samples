// Demonstrates terrain track spawning with configurable parameters.
// Provides GUI controls for adjusting track spacing and pool size.
// Manages multiple TrackSpawner components in child hierarchy.

#include "TracksSample.h"

REGISTER_COMPONENT(TracksSample);

using namespace Unigine;
using namespace Math;

// Track spawner components are collected and GUI is initialized.
void TracksSample::init()
{
	// All TrackSpawner components in children are gathered
	getComponentsInChildren<TrackSpawner>(getNode(), track_spawners);

	// Control panel is created
	init_gui();
}

// GUI resources are released.
void TracksSample::shutdown()
{
	shutdown_gui();
}

// Parameter controls are created for track spawning configuration.
void TracksSample::init_gui()
{
	// Sample window is created from reusable component
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	// Minimum distance parameter row is created
	auto hbox = WidgetHBox::create(4);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	auto label = WidgetLabel::create("Min distance between track maps");
	label->setWidth(200);
	hbox->addChild(label, Gui::ALIGN_LEFT);

	// Float input for minimum distance is created
	min_distance_edit_line = WidgetEditLine::create("0.1");
	min_distance_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	min_distance_edit_line->getEventChanged().connect(this, &TracksSample::min_distance_edit_line_callback);
	hbox->addChild(min_distance_edit_line, Gui::ALIGN_LEFT);

	// Maximum count parameter row is created
	hbox = WidgetHBox::create(4);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	label = WidgetLabel::create("Max number of track maps");
	label->setWidth(200);
	hbox->addChild(label, Gui::ALIGN_LEFT);

	// Integer input for pool size is created
	max_number_edit_line = WidgetEditLine::create("100");
	max_number_edit_line->setValidator(Gui::VALIDATOR_UINT);
	max_number_edit_line->getEventChanged().connect(this, &TracksSample::max_number_edit_line_callback);
	hbox->addChild(max_number_edit_line, Gui::ALIGN_LEFT);
}

// Sample window is released.
void TracksSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

// Distance parameter is applied to all spawners.
void TracksSample::min_distance_edit_line_callback()
{
	// Input text is parsed as float
	float value = String::atof(min_distance_edit_line->getText());

	// Value is applied to all spawner components
	for (auto track_spawner : track_spawners)
	{
		track_spawner->setMinDistanceBetweenTracks(value);
	}
}

// Pool size parameter is applied to all spawners.
void TracksSample::max_number_edit_line_callback()
{
	// Input text is parsed as integer
	int value = String::atoi(max_number_edit_line->getText());

	// Value is applied to all spawner components
	for (auto track_spawner : track_spawners)
	{
		track_spawner->setMaxNumberOfTracks(value);
	}
}
