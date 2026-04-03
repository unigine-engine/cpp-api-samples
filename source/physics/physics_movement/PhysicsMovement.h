#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

// Controls rigid body movement using physical forces and torques.
// Supports keyboard input for driving, braking, and jumping.
class PhysicsMover: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicsMover, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);

	PROP_PARAM(Float, force, 10.0f);              // Forward/backward acceleration
	PROP_PARAM(Float, torque, 1.0f);              // Turning strength
	PROP_PARAM(Float, jump_impulse, 3.0f);        // Vertical impulse on jump
	PROP_PARAM(Float, max_speed, 30.0f);          // Linear velocity limit
	PROP_PARAM(Float, max_rotation_speed, 10.0f); // Angular velocity limit
	PROP_PARAM(Float, brakes_strength, 5.0f);     // Damping when braking

private:
	void init();           // Caches rigid body reference
	void update();         // Reads keyboard input
	void update_physics(); // Applies forces and torques

private:
	Unigine::BodyRigidPtr body; // Rigid body to control

	Unigine::Math::vec2 input_direction; // WASD input state
	bool brake = false;                  // Shift key state
	bool jump = false;                   // Space key state
};
