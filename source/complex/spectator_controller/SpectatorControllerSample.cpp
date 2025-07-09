#include "SpectatorControllerSample.h"
#include "SpectatorController.h"
#include <UnigineGame.h>


REGISTER_COMPONENT(SpectatorControllerSample)

using namespace Unigine;

void SpectatorControllerSample::init()
{
	spectator = getComponentInChildren<SpectatorController>(Game::getPlayer());

	mouse_handle_at_init = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	is_controlled = spectator->is_controlled > 0;
	is_collided = spectator->is_collided > 0;

	current_mouse_sensetivity = spectator->mouse_sensitivity;
	current_turning = spectator->turning;
	current_velocity = spectator->min_velocity;
	current_sprint_velocity = spectator->max_velocity;

	description_window.createWindow();

	description_window.addFloatParameter("Mouse sensetivity",
		"Mouse sensetivity", 
		current_mouse_sensetivity, 
		min_mouse_sensetivity, 
		max_mouse_sensetivity, 
		[this](float value) {
			current_mouse_sensetivity = value;
			spectator->mouse_sensitivity = current_mouse_sensetivity;
		});

	description_window.addFloatParameter("Turning speed",
		"Turning speed",
		current_turning,
		min_turning,
		max_turning,
		[this](float value) {
			current_turning = value;
			spectator->turning = current_turning;
		});

	description_window.addFloatParameter("Velocity",
		"Velocity",
		current_velocity,
		min_velocity,
		max_velocity,
		[this](float value) {
			current_velocity = value;
			spectator->min_velocity = current_velocity;
		});

	description_window.addFloatParameter("Sprint velocity",
		"Sprint velocity",
		current_sprint_velocity,
		min_sprint_velocity,
		max_sprint_velocity,
		[this](float value) {
			current_sprint_velocity = value;
			spectator->max_velocity = current_sprint_velocity;
		});

	WidgetGroupBoxPtr parameters = description_window.getParameterGroupBox();
	
	WidgetCheckBoxPtr controlls_checkbox = WidgetCheckBox::create("Set camera receive inputs");
	parameters->addChild(controlls_checkbox, Gui::ALIGN_LEFT);
	controlls_checkbox->setChecked(is_controlled);

	controlls_checkbox->getEventClicked().connect(*this, [this, controlls_checkbox]() 
	{
		is_controlled = controlls_checkbox->isChecked();
		spectator->is_controlled = is_controlled;
	});

	WidgetCheckBoxPtr collisions_checkbox = WidgetCheckBox::create("Set camera collision");
	parameters->addChild(collisions_checkbox, Gui::ALIGN_LEFT);
	collisions_checkbox->setChecked(is_collided);

	collisions_checkbox->getEventClicked().connect(*this, [this, collisions_checkbox]() 
	{
		is_collided = collisions_checkbox->isChecked();
		spectator->is_collided = is_collided;
	});
}

void SpectatorControllerSample::shutdown()
{
	Input::setMouseHandle(mouse_handle_at_init);

	description_window.shutdown();
}