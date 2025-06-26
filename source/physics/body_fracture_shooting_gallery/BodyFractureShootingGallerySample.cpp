#include "BodyFractureShootingGallerySample.h"

REGISTER_COMPONENT(BodyFractureShootingGallerySample);

using namespace Unigine;

void BodyFractureShootingGallerySample::init()
{
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
}

void BodyFractureShootingGallerySample::shutdown()
{
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
}
