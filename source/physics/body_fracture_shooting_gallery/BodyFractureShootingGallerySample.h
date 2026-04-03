// Configures mouse handling for the shooting gallery sample. MOUSE_HANDLE_GRAB
// locks and hides the cursor for first-person aiming style controls.
// Normal cursor behavior is restored on shutdown.

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
