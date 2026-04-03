#pragma once

#include <UnigineComponentSystem.h>

// Laser pointer that follows mouse cursor position.
// Projects a ray from the camera through the mouse position
// and places the node at the world intersection point.
class Laser : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Laser, Unigine::ComponentBase);
	COMPONENT_UPDATE(update, -1);

private:
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();

private:
	void update();
};