#pragma once

#include "MotionMode.h"

#include <UnigineComponentSystem.h>

// Linear motion: constant speed movement towards target.
// Simplest easing function - no acceleration or deceleration.
// Movement stops abruptly when target is reached.
class LinearMotion : public MotionMode
{
public:
	COMPONENT_DEFINE(LinearMotion, MotionMode);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, speed, 2.0f);	// Units per second

private:
	void update();

	// Calculate persecutor's new position and direction
	Unigine::Math::Vec3 move_towards(const Unigine::Math::Vec3 &src, // source point
		const Unigine::Math::Vec3 &dst,                              // destination point
		float step,                                                  // max offset
		bool &out_finished,                                          // movement stopped
		Unigine::Math::Vec3 &out_dir); // direction from source to destination point
};