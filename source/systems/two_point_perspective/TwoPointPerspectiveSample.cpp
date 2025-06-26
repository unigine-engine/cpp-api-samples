#include "TwoPointPerspectiveSample.h"

#include "CameraShiftController.h"

#include <UnigineWidgets.h>

REGISTER_COMPONENT(TwoPointPerspectiveSample);

using namespace Unigine;
using namespace Math;

void TwoPointPerspectiveSample::init()
{
	camera_controller = getComponent<CameraShiftController>(controlledPlayer.get());
	if (!camera_controller)
		Log::error(
			"TwoPointPerspectiveSample::init(): cannot find CameraShiftController component!\n");
	init_gui();
}

void TwoPointPerspectiveSample::shutdown()
{
	shutdown_gui();
}

void TwoPointPerspectiveSample::init_gui()
{
	sample_description_window.createWindow();

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
	camera_controller->setModeShiftModeEnabled(checkbox->isChecked());
}
