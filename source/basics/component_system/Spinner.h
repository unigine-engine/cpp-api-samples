#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>

class Spinner : public Unigine::ComponentBase
{
public:
	// methods
	COMPONENT_DEFINE(Spinner, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// parameters
	PROP_PARAM(Float, turn_speed, 30.0f);
	PROP_PARAM(Float, acceleration, 5.0f);

	PROP_PARAM(Node, spawn_node);
	PROP_PARAM(Float, min_spawn_delay, 1.0f);
	PROP_PARAM(Float, max_spawn_delay, 4.0f);

protected:
	// world main loop
	void init();
	void update();

private:
	// converter from HSV to RGB color model
	Unigine::Math::vec3 hsv2rgb(float h, float s, float v);

private:
	float start_turn_speed = 0;
	float color_offset = 0;
	float time_to_spawn = 0;
	Unigine::MaterialPtr material;
};
