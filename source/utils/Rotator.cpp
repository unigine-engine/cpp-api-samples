#include "Rotator.h"
#include <UnigineGame.h>

REGISTER_COMPONENT(Rotator);

using namespace Unigine;
using namespace Math;

void Rotator::update()
{
	vec3 delta = angular_velocity.get() * Game::getIFps();
	node->setRotation(node->getRotation() * quat{delta.x, delta.y, delta.z});
}