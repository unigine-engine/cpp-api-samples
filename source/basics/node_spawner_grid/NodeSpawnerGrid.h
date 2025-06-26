#pragma once
#include <UnigineVector.h>
#include <UnigineNode.h>
namespace Utils
{
	void spawnGrid(const char* node_path, const Unigine::Math::vec2 &cell_size, const Unigine::Math::ivec2 &grid_size,
		const Unigine::Math::Mat4 &world_transform,
		bool pivot_at_center = false,
		Unigine::Vector<Unigine::NodePtr>* output = nullptr);
}