// Sample UI for the Manipulators component. Provides checkboxes to enable/disable
// individual axes for translation, rotation, and scale operations, plus toggle buttons
// to switch between world and local coordinate basis for the manipulator widgets.

#include "ManipulatorsSample.h"

#include "Manipulators.h"

REGISTER_COMPONENT(ManipulatorsSample);

using namespace Unigine;
using namespace Math;

// Control panel UI is created with checkboxes and buttons for manipulator settings.
void ManipulatorsSample::init()
{
	// Store mouse handle for restoration on shutdown
	previous_handle = Input::getMouseHandle();

	// Get reference to Manipulators component on same node
	component = getComponent<Manipulators>(node);
	if (!component)
	{
		Log::error(
			"ManipulatorsSample::init: cannot find WidgetManipulators component!\n");
	}

	// Default to world basis mode
	is_local_basis = false;
	sample_description_window.createWindow();
	WidgetWindowPtr window = sample_description_window.getWindow();

	auto group_box = WidgetGroupBox::create("Parameters", 8, 2);
	window->addChild(group_box, Gui::ALIGN_LEFT);

	auto hBox = WidgetHBox::create(10);
	group_box->addChild(hBox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create("Translation:");
	label->setFontWrap(1);
	label->setFontRich(1);
	label->setWidth(100);
	hBox->addChild(label, Gui::ALIGN_LEFT);
	x_axis_translation_check_box = WidgetCheckBox::create("X");
	x_axis_translation_check_box->setChecked(true);
	x_axis_translation_check_box->getEventChanged().connect(this, &ManipulatorsSample::x_axis_translation_check_box_callback);
	hBox->addChild(x_axis_translation_check_box, Gui::ALIGN_LEFT);
	y_axis_translation_check_box = WidgetCheckBox::create("Y");
	y_axis_translation_check_box->setChecked(true);
	y_axis_translation_check_box->getEventChanged().connect(this, &ManipulatorsSample::y_axis_translation_check_box_callback);
	hBox->addChild(y_axis_translation_check_box, Gui::ALIGN_LEFT);
	z_axis_translation_check_box = WidgetCheckBox::create("Z");
	z_axis_translation_check_box->setChecked(true);
	z_axis_translation_check_box->getEventChanged().connect(this, &ManipulatorsSample::z_axis_translation_check_box_callback);
	hBox->addChild(z_axis_translation_check_box, Gui::ALIGN_LEFT);

	hBox = WidgetHBox::create(10);
	group_box->addChild(hBox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Rotation:");
	label->setFontWrap(1);
	label->setFontRich(1);
	label->setWidth(100);
	hBox->addChild(label, Gui::ALIGN_LEFT);
	x_axis_rotation_check_box = WidgetCheckBox::create("X");
	x_axis_rotation_check_box->setChecked(true);
	x_axis_rotation_check_box->getEventChanged().connect(this, &ManipulatorsSample::x_axis_rotation_check_box_callback);
	hBox->addChild(x_axis_rotation_check_box, Gui::ALIGN_LEFT);
	y_axis_rotation_check_box = WidgetCheckBox::create("Y");
	y_axis_rotation_check_box->setChecked(true);
	y_axis_rotation_check_box->getEventChanged().connect(this, &ManipulatorsSample::y_axis_rotation_check_box_callback);
	hBox->addChild(y_axis_rotation_check_box, Gui::ALIGN_LEFT);
	z_axis_rotation_check_box = WidgetCheckBox::create("Z");
	z_axis_rotation_check_box->setChecked(true);
	z_axis_rotation_check_box->getEventChanged().connect(this, &ManipulatorsSample::z_axis_rotation_check_box_callback);
	hBox->addChild(z_axis_rotation_check_box, Gui::ALIGN_LEFT);

	hBox = WidgetHBox::create(10);
	group_box->addChild(hBox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Scale:");
	label->setFontWrap(1);
	label->setFontRich(1);
	label->setWidth(100);
	hBox->addChild(label, Gui::ALIGN_LEFT);
	x_axis_scale_check_box = WidgetCheckBox::create("X");
	x_axis_scale_check_box->setChecked(true);
	x_axis_scale_check_box->getEventChanged().connect(this, &ManipulatorsSample::x_axis_scale_check_box_callback);
	hBox->addChild(x_axis_scale_check_box, Gui::ALIGN_LEFT);
	y_axis_scale_check_box = WidgetCheckBox::create("Y");
	y_axis_scale_check_box->setChecked(true);
	y_axis_scale_check_box->getEventChanged().connect(this, &ManipulatorsSample::y_axis_scale_check_box_callback);
	hBox->addChild(y_axis_scale_check_box, Gui::ALIGN_LEFT);
	z_axis_scale_check_box = WidgetCheckBox::create("Z");
	z_axis_scale_check_box->setChecked(true);
	z_axis_scale_check_box->getEventChanged().connect(this, &ManipulatorsSample::z_axis_scale_check_box_callback);
	hBox->addChild(z_axis_scale_check_box, Gui::ALIGN_LEFT);

	hBox = WidgetHBox::create(10);
	group_box->addChild(hBox, Gui::ALIGN_LEFT);
	world_basis_button = WidgetButton::create("World");
	hBox->addChild(world_basis_button, Gui::ALIGN_LEFT);
	world_basis_button->setToggleable(true);
	world_basis_button->setToggled(true);
	world_basis_button->getEventChanged().connect(this, &ManipulatorsSample::world_basis_button_callback);
	local_basis_button = WidgetButton::create("Local");
	hBox->addChild(local_basis_button, Gui::ALIGN_LEFT);
	local_basis_button->setToggleable(true);
	local_basis_button->setToggled(false);
	local_basis_button->getEventChanged().connect(this, &ManipulatorsSample::local_basis_button_callback);

	window->arrange();
}

// Mouse handle is restored and UI window is released.
void ManipulatorsSample::shutdown()
{
	Input::setMouseHandle(previous_handle);

	sample_description_window.shutdown();
}

// Rotation axis checkbox callbacks - toggle axis availability on rotator manipulator.
void ManipulatorsSample::x_axis_rotation_check_box_callback()
{
	component->setXAxisRotation(x_axis_rotation_check_box->isChecked());
}

void ManipulatorsSample::y_axis_rotation_check_box_callback()
{
	component->setYAxisRotation(y_axis_rotation_check_box->isChecked());
}

void ManipulatorsSample::z_axis_rotation_check_box_callback()
{
	component->setZAxisRotation(z_axis_rotation_check_box->isChecked());
}

// Translation axis checkbox callbacks - toggle axis availability on translator manipulator.
void ManipulatorsSample::x_axis_translation_check_box_callback()
{
	component->setXAxisTranslation(x_axis_translation_check_box->isChecked());
}

void ManipulatorsSample::y_axis_translation_check_box_callback()
{
	component->setYAxisTranslation(y_axis_translation_check_box->isChecked());
}

void ManipulatorsSample::z_axis_translation_check_box_callback()
{
	component->setZAxisTranslation(z_axis_translation_check_box->isChecked());
}

// Scale axis checkbox callbacks - toggle axis availability on scaler manipulator.
void ManipulatorsSample::x_axis_scale_check_box_callback()
{
	component->setXAxisScale(x_axis_scale_check_box->isChecked());
}

void ManipulatorsSample::y_axis_scale_check_box_callback()
{
	component->setYAxisScale(y_axis_scale_check_box->isChecked());
}

void ManipulatorsSample::z_axis_scale_check_box_callback()
{
	component->setZAxisScale(z_axis_scale_check_box->isChecked());
}

// Local basis button toggles manipulator to use object's local coordinate system.
void ManipulatorsSample::local_basis_button_callback()
{
	is_local_basis = local_basis_button->isToggled();
	// Ensure mutual exclusivity between toggle buttons
	world_basis_button->setToggled(!is_local_basis);
	component->setLocalBasis(is_local_basis);
}

// World basis button toggles manipulator to use world coordinate system.
void ManipulatorsSample::world_basis_button_callback()
{
	is_local_basis = !world_basis_button->isToggled();
	// Ensure mutual exclusivity between toggle buttons
	local_basis_button->setToggled(is_local_basis);
	component->setLocalBasis(is_local_basis);
}
