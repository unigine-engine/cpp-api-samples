#pragma once

#include <UnigineComponentSystem.h>

class Rotator : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Rotator, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Vec3, angular_velocity);

private:
	void update();
};