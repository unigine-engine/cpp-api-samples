// Provides UI for adjusting PhysicsMover parameters. Sliders modify force,
// torque, impulse, speed limits, and braking in real-time.

#include "PhysicsMovementSample.h"

#include "PhysicsMovement.h"

REGISTER_COMPONENT(PhysicsMovementSample);

using namespace Unigine;
using namespace Math;

// PhysicsMover component is retrieved and UI is initialized.
void PhysicsMovementSample::init()
{
	car_node = car;

	physics_mover = ComponentSystem::get()->getComponent<PhysicsMover>(car_node);

	init_gui();
}

// UI window is closed.
void PhysicsMovementSample::shutdown()
{
	shutdown_gui();
}

// Parameter sliders are created for movement tuning.
void PhysicsMovementSample::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	sample_description_window.addFloatParameter("Force", nullptr, physics_mover->force, 1.0f, 20.f,
		[this](float v) {
			physics_mover->force = v;
		});

	sample_description_window.addFloatParameter("Torque", nullptr, physics_mover->torque, 1.0f, 5.0f,
		[this](float v) {
			physics_mover->torque = v;
		});

	sample_description_window.addFloatParameter("Jump Impulse", nullptr, physics_mover->jump_impulse, 1.0f, 50.0f,
		[this](float v) {
			physics_mover->jump_impulse = v;
		});

	sample_description_window.addFloatParameter("Max Speed", nullptr, physics_mover->max_speed, 1.0f, 50.f,
		[this](float v) {
			physics_mover->max_speed = v;
		});

	sample_description_window.addFloatParameter("Max Rotation Speed", nullptr, physics_mover->max_rotation_speed, 1.0f, 50.f,
		[this](float v) {
			physics_mover->max_rotation_speed = v;
		});

	sample_description_window.addFloatParameter("Brakes Strength", nullptr, physics_mover->brakes_strength, 1.0f, 50.f,
		[this](float v) {
			physics_mover->brakes_strength = v;
		});
}

// UI window is released.
void PhysicsMovementSample::shutdown_gui()
{
	sample_description_window.shutdown();
}
