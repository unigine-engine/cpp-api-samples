// Spawns projectiles on mouse click that can shatter destructible objects.
// Each projectile is loaded from a node file, positioned in front of the gun,
// and given an initial impulse in the firing direction.

#include "BodyFractureShootingGalleryGun.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(BodyFractureShootingGalleryGun);

using namespace Unigine;

void BodyFractureShootingGalleryGun::update()
{
	if (Console::isActive())
		return;

	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		// Load projectile from node file
		auto projectileNode = World::loadNode(projectile, true);
		if (!projectileNode)
			return;

		// Spawn projectile one unit ahead along the gun's negative Z axis (forward)
		projectileNode->setWorldPosition(node->getWorldPosition() + Math::Vec3(node->getWorldDirection(Math::AXIS_NZ)));
		auto body = projectileNode->getObjectBodyRigid();
		if (!body)
			return;

		// Apply instant velocity in the firing direction
		body->addLinearImpulse(node->getWorldDirection(Math::AXIS_NZ) * force);
	}
}
