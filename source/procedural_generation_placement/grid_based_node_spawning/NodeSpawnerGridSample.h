// Sample UI for configuring grid-based node spawning.
// Provides controls for grid dimensions, cell size, and pivot mode.
// Grid is rebuilt interactively as parameters are adjusted.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Manages UI controls and triggers grid rebuilds when parameters change.
class NodeSpawnerGridSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeSpawnerGridSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);
	PROP_PARAM(File, node_to_spawn)

private:
	void init();
	void shutdown();
	// Clears existing nodes and spawns a fresh grid with current settings
	void redraw();

private:
	// Spacing between adjacent nodes in the grid
	float cell_size = 5.0f;
	// Number of nodes along each axis
	int grid_size_x = 5;
	int grid_size_y = 5;
	SampleDescriptionWindow sample_description_window;
	// References to all spawned nodes; used for cleanup before respawning
	Unigine::Vector<Unigine::NodePtr> spawned_nodes;
	// When true, grid is centered at origin; otherwise grows from corner
	bool is_pivot_center = false;
};
