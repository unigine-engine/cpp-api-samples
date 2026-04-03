// Timer-based node spawner that periodically instantiates nodes.
// Spawn rate is configurable at runtime. Uses interval subtraction
// rather than reset to maintain consistent spawn frequency.

#pragma once
#include <UnigineComponentSystem.h>

// Spawns nodes at regular intervals based on configurable spawn rate.
class NodeSpawnerTimer :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeSpawnerTimer, ComponentBase);

	COMPONENT_UPDATE(update);

	// Interval in seconds between spawns
	PROP_PARAM(Float, spawn_rate, 5);
	// Path to .node file that will be instantiated
	PROP_PARAM(File, node_to_spawn);

	// Returns current timer progress for UI visualization
	float getTimer() const { return time_buffer_sec; }
private:
	void update();


private:
	// Accumulated time since last spawn
	float time_buffer_sec = 0.1f;
};

