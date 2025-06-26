#pragma once

#include <UnigineComponentSystem.h>

class Spawner
	: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Spawner, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(File, spawn_node_path);
	PROP_PARAM(Toggle, start_with_cooldown, false);
	PROP_PARAM(Double, spawn_interval, 1.0);
	PROP_PARAM(Vec3, spawn_spread, {0, 0, 0});

private:
	void init();
	void update();

	void spawn();

private:
	double last_spawn_time;
	bool started;
};
