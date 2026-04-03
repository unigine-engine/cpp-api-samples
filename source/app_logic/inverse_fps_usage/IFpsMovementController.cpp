// Compares frame-rate dependent vs independent movement. When use_ifps is true,
// movement is scaled by Game::getIFps() for consistent speed regardless of FPS.
// Without IFps scaling, movement speed varies with frame rate.

#include "IFpsMovementController.h"

#include <Unigine.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(IFpsMovementController);

// Node is moved along X axis; direction reverses at boundaries.
void IFpsMovementController::update()
{
	// With IFps: distance = speed * deltaTime (consistent regardless of FPS)
	// Without: distance = speed per frame (faster at higher FPS)
	if (use_ifps)
	{
		node->translate(current_dir * movement_speed * Game::getIFps());
	}
	else {
		node->translate(current_dir * movement_speed);
	}

	// Direction is reversed when node reaches boundary limits
	if (node->getWorldPosition().x > 5)
		current_dir = Vec3_left;
	if (node->getWorldPosition().x < -5)
		current_dir = Vec3_right;
}