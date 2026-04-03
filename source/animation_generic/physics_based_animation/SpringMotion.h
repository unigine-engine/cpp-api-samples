#pragma once

#include "MotionMode.h"

#include <UnigineComponentSystem.h>

// Spring-based motion using damped harmonic oscillator physics.
// Creates natural, organic-feeling movement with overshoot and settle behavior.
// Base class for SpringEasy (simple) and SpringRegular (configurable) variants.
// Uses Hooke's law: F = -kx - cv (stiffness and damping forces).
class SpringMotion : public MotionMode

{
public:
	COMPONENT_DEFINE(SpringMotion, MotionMode);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, physicsFPS, 60.0f);	// Fixed physics timestep for stability
	PROP_PARAM(Float, mass, 0.75f);			// Affects oscillation frequency

protected:
	float physics_ifps = 1.0f / 60.0f;

	float idle_anim_weight = 0;
	float run_anim_time = 0;

	bool finished = false;

	// Spring parameters: stiffness (k) and damping (c) coefficients
	float stiffness = 1.0f;	// Higher = faster snap to target
	float damping = 1.0f;	// Higher = less oscillation

	// Fixed timestep accumulator for physics simulation
	float accumulator = 0;

	// Spring state variables
	Unigine::Math::Vec3 position;
	Unigine::Math::Vec3 velocity;

	// Target position interpolation (for smooth target movement)
	Unigine::Math::Vec3 prev_in, cur_in;
	// Output position interpolation (for smooth rendering between physics steps)
	Unigine::Math::Vec3 prev_out_p, cur_out_p;
	Unigine::Math::Vec3 lerped_position;

	const float distance_epsilon = 0.5f;	// Threshold for "caught" detection

protected:
	void init();
	void update();
	void on_enable() override;

	virtual Unigine::Math::Vec3 update_physics(const Unigine::Math::Vec3 &src,
		const Unigine::Math::Vec3 &dst,
		bool &out_finished,
		Unigine::Math::Vec3 &out_dir);

	// Integrate spring physics: applies Hooke's law with damping
	void integrate_spring(const Unigine::Math::Vec3 &target_position, float ifps);

	// Derived classes must implement to set stiffness/damping values
	virtual void refresh_spring_settings() = 0;
};