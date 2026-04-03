// Moves node along a circular path by combining forward motion and rotation.
// Speed controls linear velocity; angular speed controls turning rate.
// Used for simulating vehicles that leave tracks on terrain.

#include "PathCircle.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(PathCircle);

using namespace Unigine;
using namespace Math;

// Position and rotation are updated each frame based on speed parameters.
void PathCircle::update()
{
	// Rotation is incremented around Z axis based on angular speed
	quat new_rotation = node->getRotation() * quat(0.0f, 0.0f, angular_speed.get() * Game::getIFps());

	// Position is moved forward along Y axis based on linear speed
	Vec3 new_position = node->getPosition() + Vec3(node->getDirection(AXIS::AXIS_Y)) * speed.get() * Game::getIFps();

	// Transform is composed and applied to node
	Mat4 new_transform;
	composeTransform(new_transform, new_position, new_rotation, vec3_one);
	node->setTransform(new_transform);
}
