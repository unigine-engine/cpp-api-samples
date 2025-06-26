#pragma once

#include "Flock.h"

#include <UnigineComponentSystem.h>

class FlockController final : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(FlockController, Unigine::ComponentBase)

	PROP_PARAM(Int, flock_size)

	PROP_ARRAY(Node, targets)

	PROP_PARAM(File, unit_node)

	PROP_PARAM(Float, cohesion_coefficient)
	PROP_PARAM(Float, cohesion_spot_radius)

	PROP_PARAM(Float, alignment_coefficient)

	PROP_PARAM(Float, separation_coefficient)
	PROP_PARAM(Float, separation_desired_range)

	PROP_PARAM(Float, target_coefficient)

	PROP_PARAM(Float, max_speed)
	PROP_PARAM(Float, rotation_speed)
	PROP_PARAM(Float, max_force)

	PROP_PARAM(Float, min_height)
	PROP_PARAM(Float, max_height)
	PROP_PARAM(Float, escape_force_height_padding)

	PROP_PARAM(Vec3, spawn_zone_size)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)


	UNIGINE_INLINE Flock *getFlock() { return flock.get(); }

private:
	void init();
	void update();

	Unigine::Math::Vec3 get_flock_center();
	Unigine::NodePtr get_new_target();

private:
	std::unique_ptr<Flock> flock{nullptr};

	Unigine::NodePtr flock_current_target{};
};
