#pragma once
#include <UnigineComponentSystem.h>

class BodyFractureShootingGalleryGun
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureShootingGalleryGun, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(File, projectile);
	PROP_PARAM(Float, force, 10.0f);

private:
	void update();
};
