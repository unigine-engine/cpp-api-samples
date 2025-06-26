#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>

class Car : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Car, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(updatePhysics);

	PROP_PARAM(Float, acceleration, 50.0f);
	PROP_PARAM(Float, maxVelocity, 90.0f);
	PROP_PARAM(Float, defaultTorque, 5.0f);

	PROP_PARAM(Float, turnSpeed, 50.0f);
	PROP_PARAM(Float, carBase, 3.0f);
	PROP_PARAM(Float, carWidth, 2.0f);

	PROP_PARAM(Node, wheelBl);
	PROP_PARAM(Node, wheelBr);
	PROP_PARAM(Node, wheelFl);
	PROP_PARAM(Node, wheelFr);

	void setVelocity(float v);
	void setAngle(float a);
	void setBrake(float b);

	void respawn();

protected:
	void init();
	void update();
	void updatePhysics();

private:
	Unigine::JointWheelPtr joint_wheel_bl;
	Unigine::JointWheelPtr joint_wheel_br;
	Unigine::JointWheelPtr joint_wheel_fl;
	Unigine::JointWheelPtr joint_wheel_fr;

	float current_velocity = 0.0f;
	float current_torque = 0.0f;

	float current_turn_angle = 0.0f;

	float max_turn_angle = 30.0f;
};