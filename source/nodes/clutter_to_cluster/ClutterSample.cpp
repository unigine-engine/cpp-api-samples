#include "ClutterSample.h"

#include "ClutterConverter.h"

#include <UnigineWidgets.h>

REGISTER_COMPONENT(ClutterSample);

using namespace Unigine;
using namespace Math;

void ClutterSample::init()
{
	clutter_converter = getComponent<ClutterConverter>(clutterConverter.get());
	if (!clutter_converter)
		Log::error("ClutterSample::init(): cannot find ClutterConverter component!\n");
	init_gui();
}

void ClutterSample::shutdown()
{
	sample_description_window.shutdown();
}

void ClutterSample::init_gui()
{
	sample_description_window.createWindow();

	const WidgetGroupBoxPtr &group_box = sample_description_window.getParameterGroupBox();

	auto hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_CENTER);
	auto button = WidgetButton::create("Generate Clutter");
	button->getEventClicked().connect(this, &ClutterSample::generate_button_callback);
	hbox->addChild(button, Gui::ALIGN_LEFT);
	button = WidgetButton::create("Convert to Cluster");
	button->getEventClicked().connect(this, &ClutterSample::convert_button_callback);
	hbox->addChild(button, Gui::ALIGN_LEFT);
}

void ClutterSample::generate_button_callback()
{
	clutter_converter->generateClutter();
}

void ClutterSample::convert_button_callback()
{
	clutter_converter->convertToCluster();
}
