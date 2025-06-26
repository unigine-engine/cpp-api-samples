#pragma once
#include <UnigineComponentSystem.h>

class Projectile : public Unigine::ComponentBase
{
public:
	// methods
	COMPONENT(Projectile, Unigine::ComponentBase);
	COMPONENT_UPDATE_SYNC_THREAD(updateSyncThread);
	COMPONENT_UPDATE(update);

	// property name
	PROP_NAME("Projectile");

	// parameters
	PROP_PARAM(Float, speed, 5.0f);
	PROP_PARAM(Float, lifetime, 5.0f); // lifetime of the projectile (declaration with a default value)
	PROP_PARAM(Int, damage); // damage caused by the projectile (declaration with no default value)

	// methods
	void setMaterial(const Unigine::MaterialPtr &mat);

protected:
	// world main loop
	void updateSyncThread();
	void update();
};
