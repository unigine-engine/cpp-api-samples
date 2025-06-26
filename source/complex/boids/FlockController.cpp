#include "FlockController.h"

#include <UnigineGame.h>

namespace
{
Unit generate_unit(const Unigine::String &node_path, const Unigine::Math::BoundBox &spawn_zone)
{
	const Unigine::NodePtr node = Unigine::World::loadNode(node_path);
	const Unigine::Math::Random random;
	random.setSeed(Unigine::Game::getRandom());
	Unigine::Math::Vec3 spawn_point;
	spawn_point.x = random.getFloat(spawn_zone.minimum.x, spawn_zone.maximum.x);
	spawn_point.y = random.getFloat(spawn_zone.minimum.y, spawn_zone.maximum.y);
	spawn_point.z = random.getFloat(spawn_zone.minimum.z, spawn_zone.maximum.z);
	node->setWorldPosition(spawn_point);
	auto unit = Unit(node);
	return unit;
}

std::unique_ptr<Flock> generate_flock(const Unigine::String &node_path, const int flock_size,
	const Unigine::Math::BoundBox &spawn_zone)
{
	auto flock = std::make_unique<Flock>();
	for (int i = 0; i < flock_size; ++i)
	{
		auto unit = generate_unit(node_path, spawn_zone);
		flock->addUnit(std::move(unit));
	}
	return flock;
}
} // namespace

void FlockController::init()
{
	auto spawn_zone = Unigine::Math::BoundBox(-spawn_zone_size.get(), spawn_zone_size.get());
	spawn_zone.setTransform(node->getWorldTransform());
	flock = generate_flock(unit_node.get(), flock_size, spawn_zone);
	UNIGINE_ASSERT(targets.size() > 0);
	flock_current_target = get_new_target();
	flock->setTarget(flock_current_target);
	flock->setCohesionCoefficient(cohesion_coefficient);
	flock->setAlignmentCoefficient(alignment_coefficient);
	flock->setSeparationCoefficient(separation_coefficient);
	flock->setTargetChaseValue(target_coefficient);
	for (auto &unit : flock->getUnits())
	{
		unit.setSpotRadius(cohesion_spot_radius);
		unit.setDesiredSeparationRange(separation_desired_range);
		const auto speed_offset = Unigine::Game::getRandomFloat(-2.f, 2.f);
		unit.setMaxSpeed(max_speed + speed_offset);
		unit.setMaxForce(max_force);
		unit.setRotationSpeed(rotation_speed);
		unit.setLimitByHeightEnabled(true);
		unit.setMinHeight(min_height);
		unit.setMaxHeight(max_height);
		unit.setHeightPadding(escape_force_height_padding);
	}
}

void FlockController::update()
{
	flock->update();

	if (distance2(get_flock_center(), flock_current_target->getWorldPosition()) < 30.f)
	{
		flock_current_target = get_new_target();
		flock->setTarget(flock_current_target);
	}
}

Unigine::Math::Vec3 FlockController::get_flock_center()
{
	Unigine::Math::Vec3 sum{};
	for (auto &unit : flock->getUnits())
	{
		sum += unit.getNode()->getWorldPosition();
	}

	return sum / static_cast<Unigine::Math::Scalar>(flock->getUnits().size());
}

Unigine::NodePtr FlockController::get_new_target()
{
	Unigine::NodePtr new_target = targets[Unigine::Game::getRandomInt(0, targets.size() - 1)];

	if (flock_current_target && targets.size() > 1)
		while (new_target->getID() == flock_current_target->getID())
			new_target = targets[Unigine::Game::getRandomInt(0, targets.size() - 1)];

	return new_target;
}

REGISTER_COMPONENT(FlockController)
