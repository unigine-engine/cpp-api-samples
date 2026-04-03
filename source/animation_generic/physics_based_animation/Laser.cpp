// Moves the laser pointer node to follow mouse cursor position in world space.
// Casts a ray from camera through mouse position, finds world intersection, and
// places the laser on that point (constrained to XY plane).

#include "Laser.h"

#include <UnigineGame.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(Laser);

using namespace Unigine;
using namespace Math;

// Laser position is updated to follow mouse cursor in world space.
void Laser::update()
{
	// Convert mouse screen coordinates to a 3D direction from the camera
	PlayerPtr player = Game::getPlayer();
	ivec2 mouse_coord = Input::getMousePosition();
	Vec3 mouse_dir = Vec3(player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y));

	// Cast a ray from camera through mouse position to find world intersection
	Vec3 cam_pos = player->getWorldPosition();
	if (World::getIntersection(cam_pos, cam_pos + mouse_dir * player->getZFar(), ~0, intersection))
	{
		Vec3 point = intersection->getPoint();
		// Keep laser on the same horizontal plane (XY plane movement only)
		point.z = node->getWorldPosition().z;
		node->setWorldPosition(point);
	}
}
