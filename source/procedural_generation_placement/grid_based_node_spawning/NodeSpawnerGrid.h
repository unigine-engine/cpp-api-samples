// Utility function for spawning nodes in a 2D grid pattern.
// Nodes are loaded from a .node file and positioned based on
// cell size and grid dimensions. Supports centered or corner pivot.

#pragma once
#include <UnigineVector.h>
#include <UnigineNode.h>

namespace Utils
{
	// Spawns grid_size.x * grid_size.y nodes from node_path.
	// pivot_at_center: true centers grid at world_transform origin.
	// output: optional vector to collect spawned node references.
	void spawnGrid(const char* node_path, const Unigine::Math::vec2 &cell_size, const Unigine::Math::ivec2 &grid_size,
		const Unigine::Math::Mat4 &world_transform,
		bool pivot_at_center = false,
		Unigine::Vector<Unigine::NodePtr>* output = nullptr);
}