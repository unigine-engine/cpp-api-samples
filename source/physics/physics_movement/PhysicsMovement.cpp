#include "PhysicsMovement.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(PhysicsForceMovement);
REGISTER_COMPONENT(PhysicsImpulseMovement);

//Force movement

void PhysicsForceMovement::init()
{
	rigid = node->getObjectBodyRigid();
}

void PhysicsForceMovement::update()
{
	rigid->setMaxLinearVelocity(max_speed);
	input_direction.y =static_cast<float>(Input::isKeyPressed(Input::KEY_W) - Input::isKeyPressed(Input::KEY_S));
	input_direction.x = static_cast<float>(Input::isKeyPressed(Input::KEY_A) - Input::isKeyPressed(Input::KEY_D));
	brake = Input::isKeyPressed(Input::KEY_SPACE);
}

void PhysicsForceMovement::update_physics()
{
	vec3 forward = node->getWorldDirection(AXIS_Y);
	vec3 up = node->getWorldDirection(AXIS_Z);

	rigid->addForce(forward * input_direction.y * force);
	rigid->addTorque(up * input_direction.x * sign(input_direction.y) * torque);

	rigid->setLinearDamping(brake ? brakes_strenth : 0.0f);
	rigid->setAngularDamping(brake ? brakes_strenth : 0.0f);
}

//Impulse movement

void PhysicsImpulseMovement::init()
{
	rigid = node->getObjectBodyRigid();
}

void PhysicsImpulseMovement::update()
{
	rigid->setMaxLinearVelocity(max_speed);
	input_direction.y = static_cast<float>(Input::isKeyPressed(Input::KEY_W) - Input::isKeyPressed(Input::KEY_S));
	input_direction.x = static_cast<float>(Input::isKeyPressed(Input::KEY_A) - Input::isKeyPressed(Input::KEY_D));
	brake = Input::isKeyPressed(Input::KEY_SPACE);
}

void PhysicsImpulseMovement::update_physics()
{
	vec3 forward = node->getWorldDirection(AXIS_Y);
	vec3 up = node->getWorldDirection(AXIS_Z);

	rigid->addLinearImpulse(forward * input_direction.y * linear_impulse * Physics::getIFps());
	rigid->addAngularImpulse(up * input_direction.x * sign(input_direction.y) * angular_impulse * Physics::getIFps());

	rigid->setLinearDamping(brake ? brakes_strenth : 0.0f);
	rigid->setAngularDamping(brake ? brakes_strenth : 0.0f);
}
