#include "IFpsMovementController.h"

#include <Unigine.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(IFpsMovementController);

void IFpsMovementController::update()
{
	if (use_ifps)
	{
		node->translate(current_dir * movement_speed * Game::getIFps());
	}
	else {
		node->translate(current_dir * movement_speed);
	}

	if (node->getWorldPosition().x > 5)
		current_dir = Vec3_left;
	if (node->getWorldPosition().x < -5)
		current_dir = Vec3_right;
}