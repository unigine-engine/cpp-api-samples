#include "Spawner.h"
#include "UnigineComponentSystem.h"
#include <UnigineGame.h>

REGISTER_COMPONENT(Spawner);

using namespace Unigine;

void Spawner::init()
{
	last_spawn_time = Time::getSeconds();
	started = false;
}

void Spawner::update()
{
	if (!start_with_cooldown && !started)
	{
		spawn();
		started = true;
		return;
	}

	auto time = Time::getSeconds();
	if (time - last_spawn_time < spawn_interval)
		return;

	spawn();
}

void Spawner::spawn()
{
	last_spawn_time = Time::getSeconds();

	auto spawn_node = World::loadNode(spawn_node_path);
	if (!spawn_node)
		return;

	auto spread = spawn_spread.get() * (Math::randDirection() * 2 - Math::vec3_one);
	spawn_node->setWorldPosition(node->getWorldPosition() + Math::Vec3(spread));
}
