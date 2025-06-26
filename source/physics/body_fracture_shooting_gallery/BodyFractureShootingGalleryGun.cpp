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
		auto projectileNode = World::loadNode(projectile, true);
		if (!projectileNode)
			return;

		projectileNode->setWorldPosition(node->getWorldPosition() + Math::Vec3(node->getWorldDirection(Math::AXIS_NZ)));
		auto body = projectileNode->getObjectBodyRigid();
		if (!body)
			return;

		body->addLinearImpulse(node->getWorldDirection(Math::AXIS_NZ) * force);
	}
}
