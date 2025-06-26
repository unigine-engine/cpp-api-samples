#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "TerrainMesh.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class TerrainMeshSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TerrainMeshSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, terrain_mesh_prop, "Controlled Terrain Mesh");

private:
	void init();
	void update();
	void shutdown();

	void update_terrain_mesh_parameters();

	TerrainMesh *terrain_mesh;


	Unigine::Math::Vec2 position{500.0f, 500.0f};
	Unigine::Math::vec2 scale{500.0f, 500.0f};
	float rotation{45.0f};
	Unigine::Math::ivec2 resolution{256, 256};
	int concurrent_fetches{10000};


	// Gui stuff
	void init_gui();
	void shutdown_gui();

	Unigine::WidgetWindowPtr window;
	Unigine::WidgetEditLinePtr position_x_edit_line;
	Unigine::WidgetEditLinePtr position_y_edit_line;
	Unigine::WidgetEditLinePtr size_x_edit_line;
	Unigine::WidgetEditLinePtr size_y_edit_line;;
	Unigine::WidgetEditLinePtr rotation_edit_line;
	Unigine::WidgetEditLinePtr resolution_x_edit_line;
	Unigine::WidgetEditLinePtr resolution_y_edit_line;
	Unigine::WidgetEditLinePtr concurrent_fetches_edit_line;
	Unigine::WidgetCheckBoxPtr draw_bounding_box_check_box;
	Unigine::WidgetCheckBoxPtr draw_wireframe_check_box;

	Unigine::WidgetLabelPtr status_label;

	SampleDescriptionWindow sample_description_window;

	void position_x_edit_line_callback();
	void position_y_edit_line_callback();
	void size_x_edit_line_callback();
	void size_y_edit_line_callback();
	void rotation_edit_line_callback();
	void resolution_x_edit_line_callback();
	void resolution_y_edit_line_callback();
	void concurrent_fetches_edit_line_callback();
	void draw_bounding_box_check_box_callback();
	void draw_wireframe_check_box_callback();
	void generate_button_callback();
};