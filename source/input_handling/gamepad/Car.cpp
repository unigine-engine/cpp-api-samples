// Physics-based car controller using JointWheel for wheel simulation. Implements
// Ackermann steering geometry where inner and outer wheels turn at different angles
// to prevent tire scrubbing. Front-wheel drive with motor torque and braking.

#include "Car.h"
#include <UnigineGame.h>

REGISTER_COMPONENT(Car);

using namespace Unigine;
using namespace Math;

// Target velocity is set based on input value; torque is applied when accelerating.
void Car::setVelocity(float v)
{
	if (v > 0.1)
	{
		current_torque = defaultTorque;
		current_velocity = v * maxVelocity;
	}
	else
	{
		current_velocity = 0;
		current_torque = 0;
	}
}

// Steering angle is set from input; dead zone prevents drift from stick noise.
void Car::setAngle(float a)
{
	if (Math::abs(a) > 0.2)
	{
		current_turn_angle = -1 * a * max_turn_angle;
	}
	else
	{
		current_turn_angle = 0.0f;
	}
}

// Braking is simulated by applying high angular damping to rear wheels.
// This resists wheel rotation, slowing the vehicle down.
void Car::setBrake(float b)
{
	if (b > 0.1)
	{
		joint_wheel_bl->setAngularDamping(10000.0f);
		joint_wheel_br->setAngularDamping(10000.0f);
	}
	else
	{
		joint_wheel_bl->setAngularDamping(0.0f);
		joint_wheel_br->setAngularDamping(0.0f);
	}
}

// Car is reset to initial position and orientation.
void Car::respawn()
{
	node->setTransform(translate(Vec3_up * 1.0) * rotate(Vec3_up, 90.0));
}

// Wheel joints are retrieved and chase camera is configured to follow the car.
void Car::init()
{
	// Wheel joints are retrieved from wheel node bodies
	if (wheelBl)
		joint_wheel_bl = checked_ptr_cast<JointWheel>(wheelBl->getObjectBody()->getJoint(0));

	if (wheelBr)
		joint_wheel_br = checked_ptr_cast<JointWheel>(wheelBr->getObjectBody()->getJoint(0));

	if (wheelFl)
		joint_wheel_fl = checked_ptr_cast<JointWheel>(wheelFl->getObjectBody()->getJoint(0));

	if (wheelFr)
		joint_wheel_fr = checked_ptr_cast<JointWheel>(wheelFr->getObjectBody()->getJoint(0));

	// Chase camera is configured to follow the car
	auto player = checked_ptr_cast<PlayerPersecutor>(Game::getPlayer());
	player->setFixed(1);
	player->setTarget(node);
	player->setMinDistance(8.0f);
	player->setMaxDistance(12.0f);
	player->setPosition(Vec3(10.0f, 0.0f, 6.0f));
}

// Ackermann steering angles are calculated and applied to front wheels.
void Car::update()
{
	current_velocity = clamp(current_velocity, 0.0f, maxVelocity);
	current_turn_angle = clamp(current_turn_angle, -max_turn_angle, max_turn_angle);

	// Ackermann steering geometry: inner and outer wheels turn at different angles.
	// This prevents wheel scrubbing during turns by making all wheels rotate around
	// a common point (the turn center).
	float angle_0 = current_turn_angle;
	float angle_1 = current_turn_angle;
	if (Math::abs(current_turn_angle) > Consts::EPS)
	{
		// Turn radius is calculated from steering angle and wheelbase
		float radius = carBase / Math::tan(current_turn_angle * Consts::DEG2RAD);

		// Inner wheel has smaller radius (tighter angle), outer wheel has larger radius
		float radius_0 = radius - carWidth * 0.5f;
		float radius_1 = radius + carWidth * 0.5f;

		// Radii are converted back to steering angles
		angle_0 = Math::atan(carBase / radius_0) * Consts::RAD2DEG;
		angle_1 = Math::atan(carBase / radius_1) * Consts::RAD2DEG;
	}

	// Steering angles are applied by rotating the wheel joint axes
	joint_wheel_fr->setAxis10(Math::rotateZ(angle_1).getColumn3(0));
	joint_wheel_fl->setAxis10(Math::rotateZ(angle_0).getColumn3(0));
}

// Wheel motor parameters are updated at fixed physics timestep.
void Car::updatePhysics()
{
	// Front-wheel drive: target angular velocity is set for front wheels
	joint_wheel_fl->setAngularVelocity(current_velocity);
	joint_wheel_fr->setAngularVelocity(current_velocity);

	// Torque determines how strongly the motor can reach target velocity
	joint_wheel_fl->setAngularTorque(current_torque);
	joint_wheel_fr->setAngularTorque(current_torque);
}
