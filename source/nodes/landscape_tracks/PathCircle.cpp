#include "PathCircle.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(PathCircle);

using namespace Unigine;
using namespace Math;


void PathCircle::update()
{
	quat new_rotation = node->getRotation() * quat(0.0f, 0.0f, angular_speed.get() * Game::getIFps());
	Vec3 new_position = node->getPosition() + Vec3(node->getDirection(AXIS::AXIS_Y)) * speed.get() * Game::getIFps();

	Mat4 new_transform;
	composeTransform(new_transform, new_position, new_rotation, vec3_one);
	node->setTransform(new_transform);
}
