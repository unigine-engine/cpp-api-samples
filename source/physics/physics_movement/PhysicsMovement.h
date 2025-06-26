#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>


class PhysicsForceMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicsForceMovement, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);

	PROP_PARAM(Float, force, 10.0f);
	PROP_PARAM(Float, torque, 1.0f);
	PROP_PARAM(Float, max_speed, 30.0f);
	PROP_PARAM(Float, brakes_strenth, 5.0f);

private:
	void init();
	void update();
	void update_physics();

	Unigine::Math::vec2 input_direction;
	Unigine::BodyRigidPtr rigid;
	bool brake = false;
};


class PhysicsImpulseMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicsImpulseMovement, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);

	PROP_PARAM(Float, linear_impulse, 10.0f);
	PROP_PARAM(Float, angular_impulse, 1.0f);
	PROP_PARAM(Float, max_speed, 30.0f);
	PROP_PARAM(Float, brakes_strenth, 5.0f);

private:
	void init();
	void update();
	void update_physics();

	Unigine::Math::vec2 input_direction;
	Unigine::BodyRigidPtr rigid;
	bool brake = false;
};