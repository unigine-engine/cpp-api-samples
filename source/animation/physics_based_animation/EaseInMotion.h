#pragma once

#include "MotionMode.h"

#include <UnigineComponentSystem.h>

class EaseInMotion : public MotionMode

{
public:
	COMPONENT_DEFINE(EaseInMotion, MotionMode);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// frame-independent physics
	PROP_PARAM(Float, physicsFPS, 60.0f);
	PROP_PARAM(Float, acceleration, 3.0f);

private:
	float physics_ifps = 1.0f / 60.0f;
	// dynamic variables
	float velocity = 0;
	// starting drift
	float drift_vel = 4.0f;
	// persecutor has reached the target
	bool finished = false;

	// counter for the next physical frame
	float accumulator = 0;

	// target position
	Unigine::Math::Vec3 prev_in, cur_in;
	// spring positions in physical frame
	Unigine::Math::Vec3 prev_pos, cur_pos;

private:
	void init();
	void update();
	void on_enable() override;
	virtual Unigine::Math::Vec3 update_physics(const Unigine::Math::Vec3 &src, // source point
		const Unigine::Math::Vec3 &dst,                                        // destination point
		Unigine::Math::Vec3 &out_dir); // direction from source to destination point

	Unigine::Math::Vec3 move_towards(const Unigine::Math::Vec3 &src, const Unigine::Math::Vec3 &dst,
		float ifps);
};