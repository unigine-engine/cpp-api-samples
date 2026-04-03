// Simulates an expanding spherical explosion that applies outward forces to
// nearby rigid bodies and triggers fracturing on destructible objects.
// Force strength decreases as the radius grows (inverse falloff from center).

#pragma once

#include <UnigineComponentSystem.h>

class BodyFractureExplosion
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureExplosion, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	// Physics update for consistent force application regardless of frame rate
	COMPONENT_UPDATE_PHYSICS(updatePhysics);
	COMPONENT_SHUTDOWN(shutdown);

	// Enable visualizer debug rendering of explosion sphere
	PROP_PARAM(Toggle, debug, true);
	// Maximum expansion radius before explosion resets
	PROP_PARAM(Float, max_radius, 10.0f);
	// Expansion rate in units per second
	PROP_PARAM(Float, speed, 100.0f);
	// Base force magnitude applied at explosion center
	PROP_PARAM(Float, power, 100.0f);

	// Triggers the explosion from current position
	void explode();

private:
	void init();
	void updatePhysics();
	void shutdown();

private:
	// Current explosion radius (expands from 0 to max_radius)
	float radius;
	// Active explosion state flag
	bool isExploding;
	// Original visualizer state for restoration
	bool visualizer_enabled;
};
