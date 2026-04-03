// Implements spring-based motion with fixed timestep physics simulation.
// Uses accumulator pattern for frame-rate independent physics and interpolates
// between physics steps for smooth rendering. Spring follows Hooke's law.

#include "SpringMotion.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

// Base class and spring settings are initialized, then component is enabled.
void SpringMotion::init()
{
	MotionMode::init();
	physics_ifps = 1.0f / physicsFPS;
	refresh_spring_settings();
	on_enable();
}

// Persecutor position is updated using spring physics and animation is applied.
void SpringMotion::update()
{
	Vec3 cur_pos = persecutor->getPosition();
	Vec3 prev_pos = cur_pos;
	Vec3 target_pos = targetNode.get()->getWorldPosition();
	// Align target to persecutor's plane
	target_pos.z = cur_pos.z;

	// Calculate persecutor's new position and direction
	finished = false;
	Vec3 target_dir;
	cur_pos = update_physics(cur_pos, target_pos, finished, target_dir);

	// Apply calculations to persecutor
	if (!finished)
		persecutor->setRotation(target_dir);

	persecutor->setPosition(cur_pos);

	float real_speed = (float)length(cur_pos - prev_pos) / Game::getIFps();
	persecutor->setAnimation(real_speed, finished);
}

// Called when component is activated. Spring state is reset to current position.
void SpringMotion::on_enable()
{
	// Refresh persecutor's position to continue movement from its last point
	cur_out_p = persecutor->getPosition();
	prev_out_p = cur_out_p;
	position = cur_out_p;
	velocity = Vec3_zero;
}

// Fixed timestep spring physics is simulated with accumulator pattern.
Vec3 SpringMotion::update_physics(const Vec3 &src, const Vec3 &dst, bool &out_finished,
	Vec3 &out_dir)
{
	Vec3 offset = dst - src;
	float dist = (float)length(offset);
	if (dist > 0)
		out_dir = offset / dist;

	out_finished = dist < distance_epsilon;

	float ifps = Game::getIFps();
	// Clamp delta time to avoid "spiral of death" on frame spikes
	ifps = min(ifps, 0.25f);

	// Fixed timestep accumulator pattern for deterministic physics
	accumulator += ifps;
	if (accumulator >= physics_ifps)
	{
		// Store previous and current target for input interpolation
		prev_in = cur_in;
		cur_in = dst;

		// Run multiple physics steps if needed (catch-up)
		int steps = ftoi(Math::floor(accumulator / physics_ifps));
		for (int i = 0; i < steps; i++)
		{
			// Interpolate target position for sub-frame accuracy
			float alpha_in = itof(i + 1) / steps;

			prev_out_p = cur_out_p;
			integrate_spring(lerp(prev_in, cur_in, alpha_in), physics_ifps);
			cur_out_p = position;
		}
		accumulator -= steps * physics_ifps;
	}

	// Interpolate between physics frames for smooth rendering
	float alpha_out = accumulator / physics_ifps;
	lerped_position = lerp(prev_out_p, cur_out_p, alpha_out);

	return lerped_position;
}

// Single spring physics step is integrated using Hooke's law and damping.
void SpringMotion::integrate_spring(const Unigine::Math::Vec3 &target_position, float ifps)
{
	// Hooke's law: F_spring = -k * x (displacement from target)
	Vec3 force_spring = (target_position - position) * stiffness;
	// Damping force: F_damping = -c * v (opposes velocity)
	Vec3 force_damping = -velocity * damping;
	// Newton's second law: a = F/m
	Vec3 acceleration = (force_spring + force_damping) / mass;

	// Semi-implicit Euler integration (symplectic): more stable than explicit Euler
	velocity += acceleration * ifps;
	position += velocity * ifps;
}
