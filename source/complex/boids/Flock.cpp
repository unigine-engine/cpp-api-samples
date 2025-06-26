#include "Flock.h"

#include "FlockController.h"

#include <UnigineProfiler.h>


Flock::Flock()
{
	updater = std::make_unique<FlockUpdater>(this);
}

Flock::Flock(Flock &&other)
{
	units = std::move(other.units);
	cohesion_coefficient = other.cohesion_coefficient;
	alignment_coefficient = other.alignment_coefficient;
	separation_coefficient = other.separation_coefficient;
	flock_target_node = std::move(other.flock_target_node);
	use_bound = other.use_bound;
	updater = std::move(other.updater);
}

void Flock::addUnit(Unit unit)
{
	units.push_back(std::move(unit));
}

void Flock::update()
{
	updater->run();
}

Flock::FlockUpdater::FlockUpdater(Flock *flock)
	: flock(flock)
{}

Flock::FlockUpdater::~FlockUpdater() = default;

void Flock::FlockUpdater::process(int thread_num, int num_threads)
{
	if (!flock)
		return;

	while (true)
	{
		const int index = counter.fetch_add(1);
		if (index >= flock->getUnits().size())
			break;
		auto &unit = flock->getUnits().get(index);
		unit.update(flock);
	}
}

void Flock::FlockUpdater::run()
{
	counter = 0;
	runSync();
}
