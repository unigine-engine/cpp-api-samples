// User interface for clutter-to-cluster conversion demo. Provides interactive
// controls for regenerating procedural clutter placement and converting current
// instances to a static cluster for improved runtime performance.

#include "ClutterSample.h"

#include "ClutterConverter.h"

#include <UnigineWidgets.h>

REGISTER_COMPONENT(ClutterSample);

using namespace Unigine;
using namespace Math;

void ClutterSample::init()
{
	// Retrieve the ClutterConverter component from the referenced node
	clutter_converter = getComponent<ClutterConverter>(clutterConverter.get());
	if (!clutter_converter)
		Log::error("ClutterSample::init(): cannot find ClutterConverter component!\n");
	init_gui();
}

void ClutterSample::shutdown()
{
	// Clean up widget resources when component is destroyed
	sample_description_window.shutdown();
}

void ClutterSample::init_gui()
{
	// Create the main sample window container
	sample_description_window.createWindow();

	// Get the parameter area where interactive controls are placed
	const WidgetGroupBoxPtr &group_box = sample_description_window.getParameterGroupBox();

	// Create horizontal layout for side-by-side buttons
	auto hbox = WidgetHBox::create(5);
	group_box->addChild(hbox, Gui::ALIGN_CENTER);

	// Button to randomize clutter instance positions
	auto button = WidgetButton::create("Generate Clutter");
	button->getEventClicked().connect(this, &ClutterSample::generate_button_callback);
	hbox->addChild(button, Gui::ALIGN_LEFT);

	// Button to freeze current positions into a static cluster
	button = WidgetButton::create("Convert to Cluster");
	button->getEventClicked().connect(this, &ClutterSample::convert_button_callback);
	hbox->addChild(button, Gui::ALIGN_LEFT);
}

void ClutterSample::generate_button_callback()
{
	// Delegate to converter: assigns new random seed to clutter
	clutter_converter->generateClutter();
}

void ClutterSample::convert_button_callback()
{
	// Delegate to converter: creates cluster from current clutter state
	clutter_converter->convertToCluster();
}
