#include "PhysicsMovementSample.h"

#include "PhysicsMovement.h"


REGISTER_COMPONENT(PhysicsMovementSample);

using namespace Unigine;
using namespace Math;

void PhysicsMovementSample::init()
{
	car_node = car;

	comp_force = ComponentSystem::get()->getComponent<PhysicsForceMovement>(car_node);
	comp_impulse = ComponentSystem::get()->getComponent<PhysicsImpulseMovement>(car_node);

	switch_component(force);
	init_gui();
}

void PhysicsMovementSample::shutdown()
{
	shutdown_gui();
}

void PhysicsMovementSample::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	auto force_button = WidgetButton::create("Force");
	force_button->setToggleable(1);
	auto impulse_button = WidgetButton::create("Impulse");
	impulse_button->setToggleable(1);
	hbox->addChild(force_button);
	hbox->addChild(impulse_button);

	force_button->setToggled(true);

	force_button->getEventClicked().connect(button_connections, [this, force_button, impulse_button]() {
			if (impulse_button->isToggled() == force_button->isToggled())
			{
				impulse_button->setToggled(!force_button->isToggled());
			}
			switch_component(force_button->isToggled());
		});

	impulse_button->getEventClicked().connect(button_connections, 
		[this, impulse_button, force_button]() {
			if (impulse_button->isToggled() == force_button->isToggled())
			{
				force_button->setToggled(!impulse_button->isToggled());
			}
			switch_component(force_button->isToggled());
		});


	sample_description_window.addFloatParameter("Max Speed", "Max Speed", max_speed, 1.0f, 50.f,
		[this](float v) {
			max_speed = v;
			comp_force->max_speed = max_speed;
			comp_impulse->max_speed = max_speed;
		});

	sample_description_window.addFloatParameter("Acceleration", "Acceleration", max_speed, 1.0f, 20.f,
		[this](float v) {
			acceleration = v;
			comp_force->force = acceleration;
			comp_impulse->linear_impulse = acceleration;
		});

	sample_description_window.addFloatParameter("Turn Speed", "Turn Speed", turn_speed, 1.0f, 5.0f,
		[this](float v) {
			turn_speed = v;
			comp_force->torque = turn_speed;
			comp_impulse->angular_impulse = turn_speed;
		});
}

void PhysicsMovementSample::shutdown_gui()
{
	button_connections.disconnectAll();
	sample_description_window.shutdown();
}

void PhysicsMovementSample::switch_component(bool force)
{
	comp_force->setEnabled(force);
	comp_impulse->setEnabled(!force);
}
