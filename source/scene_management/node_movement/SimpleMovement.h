// Demonstrates three equivalent approaches to node movement:
// translate/rotate, setPosition/setRotation, and setTransform.
// All variants use WASD for movement; results are identical.

#pragma once
#include "UnigineComponentSystem.h"

// Variant 1: Uses Node::translate() and Node::rotate() methods.
class SimpleMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleMovement, ComponentBase);
	COMPONENT_UPDATE(update);

	// Movement speed in units per second
	PROP_PARAM(Float, velocity, 5.0f, "", "Linear velocity in units");
	// Rotation speed in degrees per second
	PROP_PARAM(Float, angular_velocity, 50.0f, "","Angular velocity in degrees");

private:
	void update();

	void move(const Unigine::Math::Vec3& dir);
	void turn(const Unigine::Math::vec3& angles);
};

// Variant 2: Uses Node::setPosition() and Node::setRotation() methods.
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

// Variant 3: Uses Node::setTransform() with matrix multiplication.
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
