// Applies physics-based movement using forces and impulses. Input is gathered
// in update() and applied in update_physics() for consistent physics behavior.

#include "PhysicsMovement.h"
#include <UnigineConsole.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(PhysicsMover);

using namespace Unigine;
using namespace Math;

// Rigid body is cached from the node.
void PhysicsMover::init()
{
	body = node->getObjectBodyRigid();
}

// Keyboard input is captured for movement and actions.
void PhysicsMover::update()
{
	body->setMaxLinearVelocity(max_speed);
	body->setMaxAngularVelocity(max_rotation_speed);

	if (!Console::isActive())
	{
		input_direction.y = Input::isKeyPressed(Input::KEY_W) - Input::isKeyPressed(Input::KEY_S);
		input_direction.x = Input::isKeyPressed(Input::KEY_A) - Input::isKeyPressed(Input::KEY_D);
		brake |= Input::isKeyPressed(Input::KEY_ANY_SHIFT);
		jump |= Input::isKeyDown(Input::KEY_SPACE);
	}
}

// Forces, torques, and impulses are applied based on input state.
void PhysicsMover::update_physics()
{
	vec3 forward = node->getWorldDirection(AXIS_Y);
	vec3 up = node->getWorldDirection(AXIS_Z);

	bool onGround = body->getNumContacts() != 0;

	body->addForce(forward * input_direction.y * force);
	body->addTorque(up * input_direction.x * sign(input_direction.y) * torque);

	body->setLinearDamping(brake && onGround ? brakes_strength : 0.0f);
	body->setAngularDamping(brake ? brakes_strength : 0.0f);

	if (jump && onGround)
	{
		body->addLinearImpulse(up * jump_impulse);
	}

	brake = false;
	jump = false;
}
