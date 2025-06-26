#pragma once

#include "MotionMode.h"

#include <UnigineComponentSystem.h>

class EaseOutMotion : public MotionMode
{
public:
	COMPONENT_DEFINE(EaseOutMotion, MotionMode);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, speed, 2.0f);

private:
	float idle_anim_weight = 0;
	float run_anim_time = 0;

	// max distance to the target to catch
	const float distance_epsilon = 0.5f;

private:
	void update();
	Unigine::Math::Vec3 move_towards(const Unigine::Math::Vec3 &src, // source point
		const Unigine::Math::Vec3 &dst,                              // destination point
		float speed,                                                 // movement speed
		bool &out_finished,                                          // movement stopped
		Unigine::Math::Vec3 &out_dir); // direction from source to destination point
};