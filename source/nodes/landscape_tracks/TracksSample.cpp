#include "TracksSample.h"

REGISTER_COMPONENT(TracksSample);

using namespace Unigine;
using namespace Math;

void TracksSample::init()
{
	getComponentsInChildren<TrackSpawner>(getNode(), track_spawners);

	init_gui();
}

void TracksSample::shutdown()
{
	shutdown_gui();
}

void TracksSample::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	auto hbox = WidgetHBox::create(4);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	auto label = WidgetLabel::create("Min distance between track maps");
	label->setWidth(200);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	min_distance_edit_line = WidgetEditLine::create("0.1");
	min_distance_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	min_distance_edit_line->getEventChanged().connect(this, &TracksSample::min_distance_edit_line_callback);
	hbox->addChild(min_distance_edit_line, Gui::ALIGN_LEFT);


	hbox = WidgetHBox::create(4);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	label = WidgetLabel::create("Max number of track maps");
	label->setWidth(200);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	max_number_edit_line = WidgetEditLine::create("100");
	max_number_edit_line->setValidator(Gui::VALIDATOR_UINT);
	max_number_edit_line->getEventChanged().connect(this, &TracksSample::max_number_edit_line_callback);
	hbox->addChild(max_number_edit_line, Gui::ALIGN_LEFT);

}

void TracksSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

void TracksSample::min_distance_edit_line_callback()
{
	float value = String::atof(min_distance_edit_line->getText());
	for (auto track_spawner : track_spawners)
	{
		track_spawner->setMinDistanceBetweenTracks(value);
	}
}

void TracksSample::max_number_edit_line_callback()
{
	int value = String::atoi(max_number_edit_line->getText());
	for (auto track_spawner : track_spawners)
	{
		track_spawner->setMaxNumberOfTracks(value);
	}
}
