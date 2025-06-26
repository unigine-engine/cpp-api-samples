#include "EaseOutMotion.h"

#include <UnigineGame.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(EaseOutMotion);

using namespace Unigine;
using namespace Math;

void EaseOutMotion::update()
{
	Vec3 cur_pos = persecutor->getPosition();
	Vec3 prev_pos = cur_pos;
	Vec3 target_pos = targetNode.get()->getWorldPosition();
	// align target to persecutor's plane
	target_pos.z = cur_pos.z;

	// calculate persecutor's new position and direction
	bool finished = true;
	Vec3 target_dir;
	cur_pos = move_towards(cur_pos, target_pos, speed, finished, target_dir);

	// apply calculations to persecutor
	if (!finished)
		persecutor->setRotation(target_dir);

	persecutor->setPosition(cur_pos);

	float real_speed = (float)length(cur_pos - prev_pos) / Game::getIFps();
	persecutor->setAnimation(real_speed, finished);
}

Vec3 EaseOutMotion::move_towards(const Vec3 &src, const Vec3 &dst, float speed, bool &out_finished,
	Vec3 &out_dir)
{
	// find the distance and the direction to destination
	Vec3 offset = dst - src;
	float dist = (float)length(offset);
	if (dist > 0)
		out_dir = offset / dist;

	out_finished = dist < distance_epsilon;

	// interpolate output position
	return lerp(src, dst, 1.0f - Math::exp(-speed * Game::getIFps()));
}
