#include "ZoomSample.h"

#include "ZoomController.h"


REGISTER_COMPONENT(ZoomSample);

using namespace Unigine;
using namespace Math;

void ZoomSample::init()
{
	init_components();
	mouse_grab = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	window_sample.createWindow();

	auto slider = window_sample.addFloatParameter("Zoom", "Zoom", 1.0f, 1.0f, 100.f, [this](float f) {
		zoom->udpate_zoom_factor(f);
		fov_label->setText("FOV:" + String::ftoa(player->getFov(), 2) + " deg");
		mouse_sensivity_label->setText("Mouse sensivity : " + String::ftoa(ControlsApp::getMouseSensitivity(), 3));
		render_scale_label->setText("Render distance scale: " + String::ftoa(Render::getDistanceScale(), 2));
		});

	WidgetGroupBoxPtr parameters = window_sample.getParameterGroupBox();


	fov_label = WidgetLabel::create("FOV:" + String::ftoa(player->getFov()) + " deg");
	parameters->addChild(fov_label, Gui::ALIGN_LEFT | Gui::ALIGN_EXPAND);

	mouse_sensivity_label = WidgetLabel::create("Mouse sensivity : " + String::ftoa(ControlsApp::getMouseSensitivity(), 2));
	parameters->addChild(mouse_sensivity_label, Gui::ALIGN_LEFT | Gui::ALIGN_EXPAND);

	render_scale_label = WidgetLabel::create("Render distance scale: " + String::ftoa(Render::getDistanceScale(), 2));
	parameters->addChild(render_scale_label, Gui::ALIGN_LEFT | Gui::ALIGN_EXPAND);


	WidgetButtonPtr reset_button = WidgetButton::create("Reset");
	parameters->addChild(reset_button, Gui::ALIGN_LEFT);
	reset_button->getEventClicked().connect(*this, [this, slider](const Ptr<Widget>&, int) {
		slider->setValue(1);
		});


	WidgetHBoxPtr hbox = WidgetHBox::create();
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	WidgetLabelPtr focus_label = WidgetLabel::create("Focus on:");
	hbox->addChild(focus_label);

	hbox->setSpace(2, 1);
	WidgetButtonPtr target_one = WidgetButton::create("Target 1");
	target_one->getEventClicked().connect(*this, [this](const Ptr<Widget>&, int) {
		zoom->focus_on_target(target_one_node);
		});
	hbox->addChild(target_one);

	WidgetButtonPtr target_two = WidgetButton::create("Target 2");
	target_two->getEventClicked().connect(*this, [this](const Ptr<Widget>&, int) {
		zoom->focus_on_target(target_two_node);
		});
	hbox->addChild(target_two);


	WidgetButtonPtr target_three = WidgetButton::create("Target 3");
	target_three->getEventClicked().connect(*this, [this](const Ptr<Widget>&, int) {
		zoom->focus_on_target(target_three_node);
		});
	hbox->addChild(target_three);
}

void ZoomSample::shutdown()
{
	window_sample.shutdown();
	Input::setMouseHandle(mouse_grab);

}

void ZoomSample::init_components()
{
	zoom = getComponent<ZoomController>(zoom_node);

	if (!zoom)
	{
		Log::error("ZoomInfo::init::init_components cannot find zoom_sample node!\n");
	}

	player = checked_ptr_cast<Player>(zoom_node.get());

	if (!player)
	{
		Log::error("ZoomSample::init::init_components cannot cast node to player!\n");
	}
	if (!target_one_node||!target_two_node||!target_three_node)
	{
		Log::error("ZoomSample::init::init_components targets are not assigned!\n");
	}
}