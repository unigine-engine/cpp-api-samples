#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
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
	void redraw();

private:
	float cell_size = 5.0f;
	int grid_size_x = 5;
	int grid_size_y = 5;
	SampleDescriptionWindow sample_description_window;
	Unigine::Vector<Unigine::NodePtr> spawned_nodes;
	bool is_pivot_center = false;
};
