// Sample UI for grid spawning. Provides sliders for grid dimensions and cell size,
// plus a checkbox to toggle between corner and center pivot modes. Grid is rebuilt
// each time a parameter changes by deleting and re-spawning all nodes.

#include "NodeSpawnerGridSample.h"


#include "NodeSpawnerGrid.h"

REGISTER_COMPONENT(NodeSpawnerGridSample);

using namespace Unigine;
using namespace Math;

// UI sliders and checkbox are created; initial grid is spawned.
void NodeSpawnerGridSample::init()
{
	sample_description_window.createWindow();

	// Grid width slider - triggers rebuild on change
	sample_description_window.addIntParameter("Grid Size X", "Grid Size X", 5, 1, 100, [this](int v) {
		grid_size_x = v;
		redraw();
	});

	// Grid height slider - triggers rebuild on change
	sample_description_window.addIntParameter("Grid Size Y", "Grid Size Y", 5, 1, 100, [this](int v) {
		grid_size_y = v;
		redraw();
	});

	// Cell spacing slider - triggers rebuild on change
	sample_description_window.addFloatParameter("Cell Size", "Cell Size", 2.f, 0.5f, 100.f, [this](float v) {
		cell_size = v;
		redraw();
	});

	// Pivot mode checkbox - toggles between corner and center origin
	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	auto checkbox = WidgetCheckBox::create();
	checkbox->getEventClicked().connect(*this, [this, checkbox]() {
		is_pivot_center = checkbox->isChecked();
		redraw();
	});
	auto label = WidgetLabel::create("Spawn from center");

	hbox->addChild(label);
	hbox->addChild(checkbox);

	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	redraw();
}


// Existing nodes are deleted and a fresh grid is spawned with current settings.
void NodeSpawnerGridSample::redraw()
{
	// Clean up previous grid before spawning new one
	if (!spawned_nodes.empty())
	{
		for (const auto &it : spawned_nodes)
		{
			it.deleteLater();
		}
		spawned_nodes.clear();
	}
	// Spawn grid at world origin with current parameters
	Utils::spawnGrid(node_to_spawn, vec2(cell_size), ivec2(grid_size_x, grid_size_y),
		Mat4_identity, is_pivot_center, &spawned_nodes);
}

// UI window is cleaned up on component destruction.
void NodeSpawnerGridSample::shutdown()
{
	//checkbox_connection.disconnect();
	sample_description_window.shutdown();
}
