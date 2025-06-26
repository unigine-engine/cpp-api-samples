#include "Laser.h"

#include <UnigineGame.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(Laser);

using namespace Unigine;
using namespace Math;

void Laser::update()
{
	// convert mouse coordinates to direction from the camera
	PlayerPtr player = Game::getPlayer();
	ivec2 mouse_coord = Input::getMousePosition();
	Vec3 mouse_dir = Vec3(player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y));

	// find intersection point at mouse position
	Vec3 cam_pos = player->getWorldPosition();
	if (World::getIntersection(cam_pos, cam_pos + mouse_dir * player->getZFar(), ~0, intersection))
	{
		Vec3 point = intersection->getPoint();
		point.z = node->getWorldPosition().z;
		node->setWorldPosition(point);
	}
}
