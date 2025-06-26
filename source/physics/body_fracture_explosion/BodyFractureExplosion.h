#pragma once

#include <UnigineComponentSystem.h>

class BodyFractureExplosion
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureExplosion, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE_PHYSICS(updatePhysics);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Toggle, debug, true);
	PROP_PARAM(Float, max_radius, 10.0f);
	PROP_PARAM(Float, speed, 100.0f);
	PROP_PARAM(Float, power, 100.0f);

	void explode();

private:
	void init();
	void updatePhysics();
	void shutdown();

private:
	float radius;
	bool isExploding;
	bool visualizer_enabled;
};
