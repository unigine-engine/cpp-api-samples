#pragma once
#include <UnigineComponentSystem.h>
class NodeSpawnerTimer :
	public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeSpawnerTimer, ComponentBase);

	COMPONENT_UPDATE(update);

	PROP_PARAM(Float, spawn_rate, 5);
	PROP_PARAM(File, node_to_spawn);

	float getTimer() const { return time_buffer_sec; }
private:
	void update();


private:
	float time_buffer_sec = 0.1f;
};

