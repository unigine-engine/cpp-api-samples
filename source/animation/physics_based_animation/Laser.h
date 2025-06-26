#pragma once

#include <UnigineComponentSystem.h>

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