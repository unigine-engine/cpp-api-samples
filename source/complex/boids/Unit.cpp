#include "Unit.h"

#include "Flock.h"

#include <UnigineGame.h>
#include <UnigineProfiler.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

Unit::Unit(const NodePtr &node)
{
	UNIGINE_ASSERT(node);
	unit_node = node;
}

void Unit::debug_visualize() const
{
	Visualizer::setEnabled(true);
	auto velocity_color = vec4_red;
	auto acceleration_color = vec4_blue;
	Visualizer::renderVector(getNode()->getWorldPosition(),
		getNode()->getWorldPosition() + velocity, velocity_color);
	Visualizer::renderVector(getNode()->getWorldPosition(),
		getNode()->getWorldPosition() + acceleration, acceleration_color);
}

void Unit::limit_rotation()
{
	const auto speed = velocity.length();
	velocity.normalizeValid();

	auto direction = rotateTowards(vec3(velocity), vec3(normalizeValid(velocity + acceleration)),
		rotation_speed * Game::getIFps());
	direction.normalizeValid();
	auto new_speed = (speed + max_force);
	new_speed = clamp(new_speed, static_cast<Scalar>(0.01f), max_speed);
	velocity = Vec3(direction) * new_speed;
}

void Unit::apply_movement()
{
	const auto new_position = unit_node->getWorldPosition() + velocity * Game::getIFps();
	auto direction = vec3(normalizeValid(velocity));
	auto new_transform = setTo(new_position, new_position + Vec3(direction), vec3_up, AXIS_Y);
	unit_node->setWorldTransform(new_transform);
	acceleration = Vec3_zero;
}

void Unit::update(Flock *flock)
{
	UNIGINE_ASSERT(flock);
	UNIGINE_ASSERT(unit_node);
	flocking(flock);
	if (is_limit_by_height)
		limit_by_height();
	limit_rotation();
	apply_movement();
}

void Unit::separation_loop(Data &separation_data, Unit &unit) const
{
	Vec3 separation_direction{};
	auto d2 = distance2(unit.getNode()->getWorldPosition(), getNode()->getWorldPosition());
	if (d2 > desired_separation_range * desired_separation_range)
		return;

	separation_direction = getNode()->getWorldPosition() - unit.getNode()->getWorldPosition();
	if (separation_direction.length2() == 0)
		return;

	separation_direction = separation_direction.normalizeValid();
	const auto proximity_factor = d2 / (desired_separation_range * desired_separation_range);
	if (proximity_factor > 0)
		separation_data.sum += separation_direction / proximity_factor;
	else
		separation_data.sum += separation_direction;
	separation_data.count++;
}

void Unit::align_loop(Unit::Data &align_data, Unit &unit)
{
	constexpr float align_range = 50.f;
	const Scalar distance = distance2(getNode()->getWorldPosition(),
		unit.getNode()->getWorldPosition());
	if (distance >= 0 && distance < align_range * align_range)
	{
		align_data.sum += unit.getCurrentVelocity();
		align_data.count++;
	}
}

void Unit::group_up_loop(Unit::Data &group_data, Unit &unit)
{
	const Scalar distance = distance2(unit_node->getWorldPosition(),
		unit.getNode()->getWorldPosition());
	if (distance >= 0 && distance < spot_radius * spot_radius)
	{
		group_data.sum += unit.getNode()->getWorldPosition();
		group_data.count++;
	}
}

void Unit::separation_total(Flock *flock, Unit::Data &separation_data)
{
	if (separation_data.count > 0)
	{
		separation_data.sum /= static_cast<Scalar>(separation_data.count);
		separation_data.sum.normalizeValid();
		separation_data.sum -= normalizeValid(velocity);
		separation_data.sum *= flock->getSeparationCoefficient();
	}
}

void Unit::align_total(Flock *flock, Unit::Data &align_data)
{
	if (align_data.count > 0)
	{
		align_data.sum /= static_cast<Scalar>(align_data.count);
		align_data.sum.normalizeValid();
		align_data.sum -= normalizeValid(velocity);
		align_data.sum *= flock->getAlignmentCoefficient();
	}
}

void Unit::group_up_total(Flock *flock, Unit::Data &group_data)
{
	if (group_data.count > 0)
	{
		Vec3 steer{};
		group_data.sum /= static_cast<Scalar>(group_data.count);
		steer = group_data.sum - getNode()->getWorldPosition();
		steer = steer.normalizeValid();
		const auto factor = flock->getCohesionCoefficient()
			* (1.f
				- distance2(group_data.sum, getNode()->getWorldPosition())
					/ (getSpotRadius() * getSpotRadius()));
		steer *= factor;
		group_data.sum = steer;
	}
}

void Unit::flocking(Flock *flock)
{
	UNIGINE_ASSERT(flock && "flock nullptr");

	Data align_data{};
	Data group_data{};
	Data separation_data{};

	for (auto &unit : flock->getUnits())
	{
		separation_loop(separation_data, unit);
		align_loop(align_data, unit);
		group_up_loop(group_data, unit);
	}

	separation_total(flock, separation_data);
	align_total(flock, align_data);
	group_up_total(flock, group_data);

	Vec3 tar = chase_target(flock);
	acceleration += tar;

	Vec3 sep = separation_data.sum;
	Vec3 ali = align_data.sum;
	Vec3 coh = group_data.sum;

	acceleration += sep;
	acceleration += ali;
	acceleration += coh;
}

void Unit::limit_by_height()
{
	const Vec3 current_position = getNode()->getWorldPosition();

	bool under_max = false;
	bool above_min = false;

	if (current_position.z > min_height)
		above_min = true;

	if (current_position.z < max_height)
		under_max = true;

	constexpr Scalar max_escape_force = 100.f;
	if (under_max && above_min)
	{
		// check padding
		if (current_position.z > max_height - height_padding)
		{
			const auto pad_height = max_height - height_padding;
			const auto force = max_escape_force / (1. - Math::pow(max_height / pad_height, 2))
				* (1. - Math::pow(current_position.z, 2) / Math::pow(pad_height, 2));
			acceleration += Vec3(0, 0, -force);
		}

		if (current_position.z < min_height + height_padding)
		{
			const auto pad_height = min_height + height_padding;
			const auto force = max_escape_force / (1. - Math::pow(max_height / pad_height, 2))
				* (1. - Math::pow(current_position.z, 2) / Math::pow(pad_height, 2));
			acceleration += Vec3(0, 0, -force);
		}

		return;
	}

	if (!above_min)
	{
		acceleration += Vec3_up * max_escape_force;
		return;
	}
	if (!under_max)
	{
		acceleration += Vec3_down * max_escape_force;
		return;
	}
}

Vec3 Unit::chase_target(const Flock *flock) const
{
	return (flock->getTarget()->getWorldPosition() - getNode()->getWorldPosition())
		* flock->getTargetChaseValue();
}