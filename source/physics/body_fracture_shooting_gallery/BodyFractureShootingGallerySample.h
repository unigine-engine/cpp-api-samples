#pragma once
#include <UnigineComponentSystem.h>

class BodyFractureShootingGallerySample
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BodyFractureShootingGallerySample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
