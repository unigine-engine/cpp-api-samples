#pragma once
#include <UnigineComponentSystem.h>

// Moving projectile with lifetime and collision detection.
// Demonstrates COMPONENT_UPDATE_SYNC_THREAD for thread-safe position updates,
// and World::getIntersection for bounding box collision queries.
class Projectile : public Unigine::ComponentBase
{
public:
	// Component macros
	COMPONENT(Projectile, Unigine::ComponentBase);
	COMPONENT_UPDATE_SYNC_THREAD(updateSyncThread);
	COMPONENT_UPDATE(update);

	// Property name
	PROP_NAME("Projectile");

	// Parameters
	PROP_PARAM(Float, speed, 5.0f);
	PROP_PARAM(Float, lifetime, 5.0f); // Lifetime in seconds (has default value)
	PROP_PARAM(Int, damage); // Damage dealt on hit (no default value)

	// Methods
	void setMaterial(const Unigine::MaterialPtr &mat);

protected:
	// World main loop
	void updateSyncThread();
	void update();
};
