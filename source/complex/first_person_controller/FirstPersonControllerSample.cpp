#include "FirstPersonControllerSample.h"

REGISTER_COMPONENT(FirstPersonControllerSample);

using namespace Unigine;

void FirstPersonControllerSample::init()
{
	mouse_grab_state_at_init = Input::isMouseGrab();
	mouse_handle_at_init = Input::getMouseHandle();
	Input::setMouseGrab(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

void FirstPersonControllerSample::shutdown()
{
	Input::setMouseGrab(mouse_grab_state_at_init);
	Input::setMouseHandle(mouse_handle_at_init);
}
