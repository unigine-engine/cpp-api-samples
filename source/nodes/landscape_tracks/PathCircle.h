#pragma once

#include <UnigineComponentSystem.h>

class PathCircle : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PathCircle, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);



	PROP_PARAM(Float, speed, 1.0f, "Speed");
	PROP_PARAM(Float, angular_speed, 45.0f, "Angular Speed");

private:
	void update();
};