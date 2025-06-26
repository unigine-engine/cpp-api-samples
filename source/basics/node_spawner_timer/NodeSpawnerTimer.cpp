#include "NodeSpawnerTimer.h"
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(NodeSpawnerTimer);

void NodeSpawnerTimer::update()
{
	time_buffer_sec += Game::getIFps();
	if (time_buffer_sec >= spawn_rate)
	{
		time_buffer_sec -= spawn_rate;
		NodePtr spawned_node = World::loadNode(node_to_spawn);
		spawned_node->setTransform(node->getTransform());
	}
}