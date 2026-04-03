// Configures mouse handling for the shooting gallery sample. MOUSE_HANDLE_GRAB
// locks and hides the cursor for aiming; restored to user mode on shutdown.

#include "BodyFractureShootingGallerySample.h"

REGISTER_COMPONENT(BodyFractureShootingGallerySample);

using namespace Unigine;

void BodyFractureShootingGallerySample::init()
{
	// Lock and hide cursor for first-person aiming
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

void BodyFractureShootingGallerySample::shutdown()
{
	// Restore normal cursor behavior
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
}
