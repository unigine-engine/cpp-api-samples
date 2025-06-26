#include "LinearMotion.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(LinearMotion);

using namespace Unigine;
using namespace Math;

void LinearMotion::update()
{
	Vec3 cur_pos = persecutor->getPosition();
	Vec3 target_pos = targetNode.get()->getWorldPosition();
	// align target to persecutor's plane
	target_pos.z = cur_pos.z;

	bool finished = true;
	Vec3 target_dir;
	float step = speed * Game::getIFps();
	cur_pos = move_towards(cur_pos, target_pos, step, finished, target_dir);

	// apply calculations to persecutor
	if (!finished)
		persecutor->setRotation(target_dir);

	persecutor->setPosition(cur_pos);
	persecutor->setAnimation(speed, finished);
}

Vec3 LinearMotion::move_towards(const Vec3 &src, const Vec3 &dst, float step, bool &out_finished,
	Vec3 &out_dir)
{
	// find distance and direction to destination
	Vec3 offset = dst - src;
	float dist = (float)length(offset);
	if (dist <= step)
	{
		out_finished = true;
		if (dist > 0)
			out_dir = offset / dist;
		return dst;
	}

	out_finished = false;
	out_dir = offset / dist;
	return src + out_dir * step;
}