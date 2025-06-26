#pragma once
#include "UnigineComponentSystem.h"

class SimpleMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovement, ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, velocity, 5.0f, "", "Linear velocity in units");
	PROP_PARAM(Float, angular_velocity, 50.0f, "","Angular velocity in degrees");

private:
	void update();

	void move(const Unigine::Math::Vec3& dir);
	void turn(const Unigine::Math::vec3& angles);
};

class SimpleMovement2 : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovement2, ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, velocity, 5.0f, "", "Linear velocity in units");
	PROP_PARAM(Float, angular_velocity, 50.0f, "", "Angular velocity in degrees");

private:
	void update();

	void move(const Unigine::Math::Vec3& dir);
	void turn(const Unigine::Math::vec3& angles);
};

class SimpleMovement3 : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovement3, ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, velocity, 5.0f, "", "Linear velocity in units");
	PROP_PARAM(Float, angular_velocity, 50.0f, "", "Angular velocity in degrees");

private:
	void update();

	void move(const Unigine::Math::Vec3& dir);
	void turn(const Unigine::Math::vec3& angles);
};
