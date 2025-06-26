#include "EaseOutBounceMotion.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(EaseOutBounceMotion);

using namespace Unigine;
using namespace Math;


Vec3 EaseOutBounceMotion::update_physics(const Vec3 &src, const Vec3 &dst, bool &out_finished,
	Vec3 &out_dir)
{
	// find distance and direction to destination
	Vec3 offset = dst - src;
	float dist = (float)length(offset);
	if (dist > 0)
		out_dir = offset / dist;

	out_finished = dist < distance_epsilon;

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

		// target moved
		if (prev_in != cur_in)
		{
			// calculate target normal
			wall_normal = normalize(prev_in - cur_in);
			if (dot(normalize(position - cur_in), wall_normal) < 0)
				wall_normal = -wall_normal;
		}

		// update physics
		int steps = ftoi(Math::floor(accumulator / physics_ifps));
		for (int i = 0; i < steps; i++)
		{
			// interpolate input for each physical frame: (0, 1]
			float alpha_in = itof(i + 1) / steps;
			Vec3 target_position = lerp(prev_in, cur_in, alpha_in);

			prev_out_p = cur_out_p;
			integrate_spring(target_position, physics_ifps);

			Vec3 dir_to_cat_from_target = normalize(position - target_position);
			// apply the bounce effect in case the direction is opposite and the position is close to the target
			if (dot(dir_to_cat_from_target, wall_normal) < 0.0
				&& length2(position - target_position) < bounceRadius * bounceRadius)
				velocity = -velocity;
			// recalculate position after applying the bounce effect
			new_position += velocity * physics_ifps;
			position = new_position;

			cur_out_p = position;
		}
		// reset counter
		accumulator -= steps * physics_ifps;
	}

	// interpolate output position
	float alpha_out = accumulator / physics_ifps;
	lerped_position = lerp(prev_out_p, cur_out_p, alpha_out);

	return lerped_position;
}

void EaseOutBounceMotion::on_enable()
{
	SpringMotion::on_enable();
	new_position = position;
}
