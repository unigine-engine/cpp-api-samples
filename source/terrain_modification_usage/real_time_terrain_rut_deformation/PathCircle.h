// Moves node along a circular path by combining forward motion and rotation.
// Speed controls linear velocity; angular speed controls turning rate.
// Used for simulating vehicles that leave tracks on terrain.

#pragma once

#include <UnigineComponentSystem.h>

// Animates node movement in a circular pattern.
class PathCircle : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PathCircle, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, speed, 1.0f, "Speed");				// Linear velocity in units per second
	PROP_PARAM(Float, angular_speed, 45.0f, "Angular Speed");	// Rotation rate in degrees per second

private:
	void update();
};