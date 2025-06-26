#pragma once

#include <UnigineComponentSystem.h>

class SlingRope;

class CraneControl : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CraneControl, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, turn_max_speed, 10.0f);
	PROP_PARAM(Float, turn_acceleration, 2.0f);
	PROP_PARAM(Float, turn_deceleration, 1.0f);

	PROP_PARAM(Node, rope);
	PROP_PARAM(Float, rope_speed, 2.0f);

private:
	void init();
	void update();

private:
	SlingRope *rope_component = nullptr;
	float turn_speed = 0;
};
