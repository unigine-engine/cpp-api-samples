// Spawns projectiles on mouse click that can shatter destructible objects.
// Each projectile is loaded from a node file, positioned at the gun location,
// and given an initial impulse in the firing direction.

#pragma once
#include <UnigineComponentSystem.h>

class BodyFractureShootingGalleryGun
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureShootingGalleryGun, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	// Node file (.node) containing the projectile prefab
	PROP_PARAM(File, projectile);
	// Impulse magnitude applied to projectile on spawn
	PROP_PARAM(Float, force, 10.0f);

private:
	void update();
};
