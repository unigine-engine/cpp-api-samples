#pragma once

#include "SpringRegular.h"

#include <UnigineComponentSystem.h>

class EaseOutBounceMotion : public SpringRegular

{
public:
	COMPONENT_DEFINE(EaseOutBounceMotion, SpringRegular);

	PROP_PARAM(Float, bounceRadius, 3.f);

private:
	// medium dynamic variables for position
	Unigine::Math::Vec3 new_position;
	// bounce normal
	Unigine::Math::Vec3 wall_normal;

private:
	Unigine::Math::Vec3 update_physics(const Unigine::Math::Vec3 &src, // source point
		const Unigine::Math::Vec3 &dst,                                // destination point
		bool &out_finished,                                            // movement stopped
		Unigine::Math::Vec3 &out_dir) override; // direction from source to destination point

	void on_enable() override;
};