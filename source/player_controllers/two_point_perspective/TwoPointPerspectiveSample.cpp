// Sample UI for toggling two-point perspective mode. Enables architectural
// visualization style where vertical lines stay vertical regardless of
// camera pitch angle.

#include "TwoPointPerspectiveSample.h"

#include "CameraShiftController.h"

#include <UnigineWidgets.h>

REGISTER_COMPONENT(TwoPointPerspectiveSample);

using namespace Unigine;
using namespace Math;

void TwoPointPerspectiveSample::init()
{
	// Get camera controller from the player
	camera_controller = getComponent<CameraShiftController>(controlledPlayer.get());
	if (!camera_controller)
		Log::error(
			"TwoPointPerspectiveSample::init(): cannot find CameraShiftController component!\n");

	// Create UI window with controls
	init_gui();
}

void TwoPointPerspectiveSample::shutdown()
{
	shutdown_gui();
}

void TwoPointPerspectiveSample::init_gui()
{
	// Sample UI with description and controls
	sample_description_window.createWindow();

	// Checkbox to enable/disable two-point perspective mode
	checkbox = WidgetCheckBox::create();
	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	auto label = WidgetLabel::create("Two-Point perspective: ");

	checkbox->getEventChanged().connect(this, &TwoPointPerspectiveSample::on_checkbox_changed);
	checkbox->setChecked(false);

	hbox->addChild(label, Gui::ALIGN_LEFT);
	hbox->addChild(checkbox, Gui::ALIGN_LEFT);

	parameters->addChild(hbox, Gui::ALIGN_LEFT);
}

void TwoPointPerspectiveSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

void TwoPointPerspectiveSample::on_checkbox_changed()
{
	// Toggle camera shift mode when checkbox state changes
	camera_controller->setModeShiftModeEnabled(checkbox->isChecked());
}
