// Utility for spawning nodes in a 2D grid pattern. Supports centering the grid
// at the pivot point or growing from corner. Nodes are loaded from a .node file
// and positioned based on cell size and grid dimensions.

#include "NodeSpawnerGrid.h"
#include <UnigineWorld.h>

using namespace Unigine;
using namespace Math;


// Spawns a grid of nodes from the specified .node file. When pivot_at_center is true,
// the grid is centered at the world transform origin; otherwise it grows from the corner.
void Utils::spawnGrid(const char* node_path, const Unigine::Math::vec2& cell_size, const Unigine::Math::ivec2& grid_size,
	const Unigine::Math::Mat4& world_transform,
	bool pivot_at_center,
	Unigine::Vector<Unigine::NodePtr>* output)
{
	for (int x = 0; x < grid_size.x; x++)
	{
		// Calculate X position - offset by half grid size when centering
		double x_pos = pivot_at_center ? x * cell_size.x - grid_size.x * cell_size.x/2 : x * cell_size.x;
		for (int y = 0; y < grid_size.y; y++)
		{
			// Calculate Y position - offset by half grid size when centering
			double y_pos = pivot_at_center ? y * cell_size.y - grid_size.y*cell_size.y/2 : y * cell_size.y;
			NodePtr spawned_node = World::loadNode(node_path);
			// Combine world transform with grid cell offset (Z = 0 for flat grid)
			spawned_node->setTransform(world_transform * translate(Vec3(static_cast<Scalar>(x_pos), static_cast<Scalar>(y_pos), 0)));
			if (output)
				output->push_back(spawned_node);
		}
	}
}
