// Manages mouse input state for the first-person controller sample.
// Saves and restores mouse grab/handle modes between sessions.

#include "FirstPersonControllerSample.h"

REGISTER_COMPONENT(FirstPersonControllerSample);

using namespace Unigine;

// Configures mouse for first-person control. Saves current state so it
// can be restored when sample ends, then enables grab mode for camera control.
void FirstPersonControllerSample::init()
{
	// Preserve existing mouse settings for cleanup
	mouse_grab_state_at_init = Input::isMouseGrab();
	mouse_handle_at_init = Input::getMouseHandle();
	// Start with grab disabled (user clicks to capture mouse)
	Input::setMouseGrab(false);
	// MOUSE_HANDLE_GRAB centers cursor each frame - required for FPS camera
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

// Restores mouse to its pre-sample state. Ensures editor or other
// applications regain proper mouse control after sample exits.
void FirstPersonControllerSample::shutdown()
{
	Input::setMouseGrab(mouse_grab_state_at_init);
	Input::setMouseHandle(mouse_handle_at_init);
}
