#pragma once

#include "MotionMode.h"

#include <UnigineComponentSystem.h>

class SpringMotion : public MotionMode

{
public:
	COMPONENT_DEFINE(SpringMotion, MotionMode);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// frame-independent physics
	PROP_PARAM(Float, physicsFPS, 60.0f);
	PROP_PARAM(Float, mass, 0.75f);

protected:
	float physics_ifps = 1.0f / 60.0f;
	// animation parameters
	float idle_anim_weight = 0;
	float run_anim_time = 0;

	// persecutor reached the target
	bool finished = false;

	// spring parameters
	float stiffness = 1.0f;
	float damping = 1.0f;

	// counter for next physical frame
	float accumulator = 0;

	// dynamic variables
	Unigine::Math::Vec3 position;
	Unigine::Math::Vec3 velocity;

	// target position
	Unigine::Math::Vec3 prev_in, cur_in;
	// spring positions in physical frames
	Unigine::Math::Vec3 prev_out_p, cur_out_p;
	// spring position in current render frame
	Unigine::Math::Vec3 lerped_position;

	// max distance to target for catch
	const float distance_epsilon = 0.5f;

protected:
	void init();
	void update();
	void on_enable() override;

	virtual Unigine::Math::Vec3 update_physics(const Unigine::Math::Vec3 &src, // source point
		const Unigine::Math::Vec3 &dst,                                        // destination point
		bool &out_finished,                                                    // movement stopped
		Unigine::Math::Vec3 &out_dir); // direction from source to destination point

	// calculate spring dynamic variables
	void integrate_spring(const Unigine::Math::Vec3 &target_position, float ifps);

	// renew spring parameters
	virtual void refresh_spring_settings() = 0;
};