#include "SpringMotion.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

void SpringMotion::init()
{
	MotionMode::init();
	physics_ifps = 1.0f / physicsFPS;
	refresh_spring_settings();
	on_enable();
}

void SpringMotion::update()
{
	Vec3 cur_pos = persecutor->getPosition();
	Vec3 prev_pos = cur_pos;
	Vec3 target_pos = targetNode.get()->getWorldPosition();
	// align target to persecutor's plane
	target_pos.z = cur_pos.z;

	// calculate persecutor's new position and direction
	finished = false;
	Vec3 target_dir;
	cur_pos = update_physics(cur_pos, target_pos, finished, target_dir);

	// apply calculations to persecutor
	if (!finished)
		persecutor->setRotation(target_dir);

	persecutor->setPosition(cur_pos);

	float real_speed = (float)length(cur_pos - prev_pos) / Game::getIFps();
	persecutor->setAnimation(real_speed, finished);
}

void SpringMotion::on_enable()
{
	// refresh persecutor's position to continue movement from its last point
	cur_out_p = persecutor->getPosition();
	prev_out_p = cur_out_p;
	position = cur_out_p;
	velocity = Vec3_zero;
}

Vec3 SpringMotion::update_physics(const Vec3 &src, const Vec3 &dst, bool &out_finished,
	Vec3 &out_dir)
{
	// find distance and direction to the destination point
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

		// update physics
		int steps = ftoi(Math::floor(accumulator / physics_ifps));
		for (int i = 0; i < steps; i++)
		{
			// interpolate input for each physical frame: (0, 1]
			float alpha_in = itof(i + 1) / steps;

			prev_out_p = cur_out_p;
			integrate_spring(lerp(prev_in, cur_in, alpha_in), physics_ifps);
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

void SpringMotion::integrate_spring(const Unigine::Math::Vec3 &target_position, float ifps)
{
	// calculate spring dynamics
	Vec3 force_spring = (target_position - position) * stiffness;
	Vec3 force_damping = -velocity * damping;
	Vec3 acceleration = (force_spring + force_damping) / mass;

	// use semi-implicit euler integrator
	velocity += acceleration * ifps;
	position += velocity * ifps;
}
