#include "EaseInMotion.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(EaseInMotion);

using namespace Unigine;
using namespace Math;

void EaseInMotion::init()
{
	MotionMode::init();
	on_enable();
	physics_ifps = 1.0f / physicsFPS;
}

void EaseInMotion::update()
{
	Vec3 cur_pos = persecutor->getPosition();
	Vec3 target_pos = targetNode.get()->getWorldPosition();
	// align target to persecutor's plane
	target_pos.z = cur_pos.z;

	// calculate persecutor's new position and direction
	Vec3 target_dir;
	cur_pos = update_physics(cur_pos, target_pos, target_dir);

	// apply calculations to persecutor
	if (!finished)
		persecutor->setRotation(target_dir);

	persecutor->setPosition(cur_pos);

	float speed = Math::max(drift_vel, velocity);
	persecutor->setAnimation(speed, finished);
}

void EaseInMotion::on_enable()
{
	cur_pos = persecutor->getPosition();
	velocity = 0;
	prev_pos = cur_pos;
}

Vec3 EaseInMotion::update_physics(const Vec3 &src, const Vec3 &dst, Vec3 &out_dir)
{
	// find the distance and direction to the destination point
	Vec3 offset = dst - src;
	float dist = (float)length(offset);
	if (dist > 0)
		out_dir = offset / dist;


	float ifps = Game::getIFps();
	// avoid "spiral of death"
	ifps = min(ifps, 0.25f);

	accumulator += ifps;
	// check if we've reached next physical frame
	if (accumulator >= physics_ifps)
	{
		// update input
		prev_in = cur_in;
		cur_in = dst;

		// update physics
		int steps = ftoi(Math::floor(accumulator / physics_ifps));
		for (int i = 0; i < steps; i++)
		{
			// interpolate input for each physical frame: (0, 1]
			float alpha_in = itof(i + 1) / steps;

			prev_pos = cur_pos;
			cur_pos = move_towards(cur_pos, lerp(prev_in, cur_in, alpha_in), physics_ifps);
		}
		// reset counter
		accumulator -= steps * physics_ifps;
	}

	return lerp(prev_pos, cur_pos, accumulator / physics_ifps); // return linearly interpolated position
}

Vec3 EaseInMotion::move_towards(const Vec3 &src, const Vec3 &dst, float ifps)
{
	Vec3 target_dir;

	velocity += acceleration * ifps;
	float step = velocity * ifps;

	Vec3 offset = dst - src;
	float dist = (float)length(offset);

	// can reach destination in one step
	if (dist <= step)
	{
		finished = true;
		if (dist > 0)
			target_dir = offset / dist;
		velocity = 0;
		return dst;
	}

	finished = false;
	target_dir = offset / dist;
	return src + target_dir * step;
}