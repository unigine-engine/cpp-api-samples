// Timer-based node spawner that periodically loads and instantiates a .node file.
// Spawn rate is configurable. The timer subtracts the interval rather than resetting
// to zero, ensuring consistent spawn frequency regardless of frame time variations.

#include "NodeSpawnerTimer.h"
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(NodeSpawnerTimer);

// Time is accumulated each frame; node is spawned when interval is reached.
void NodeSpawnerTimer::update()
{
	// Accumulate frame time until spawn interval is reached
	time_buffer_sec += Game::getIFps();
	if (time_buffer_sec >= spawn_rate)
	{
		// Subtract interval (not reset) to maintain timing accuracy across frames
		time_buffer_sec -= spawn_rate;
		NodePtr spawned_node = World::loadNode(node_to_spawn);
		// Spawn at this component's transform location
		spawned_node->setTransform(node->getTransform());
	}
}