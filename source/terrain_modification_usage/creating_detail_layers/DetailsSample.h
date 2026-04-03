// Demonstrates runtime creation of terrain detail layers on ObjectLandscapeTerrain.
// Detail materials for grass, stone, and snow are created and applied to mask channels.
// Render order is configured to layer details correctly (stone, grass, then snow caps).

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

// Creates and configures terrain detail layers at runtime.
class DetailsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DetailsSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, terrain_param, "Terrain");				// Reference to ObjectLandscapeTerrain
	PROP_PARAM(File, grass_texture_param, "Grass Texture");	// Albedo texture for grass detail
	PROP_PARAM(File, stone_texture_param, "Stone Texture");	// Albedo texture for stone detail

private:
	void init();
	void shutdown();

	void create_detail_grass();			// Creates grass detail on mask channel 1
	void create_detail_stone();			// Creates stone detail on mask channel 2
	void create_detail_snow_cap();		// Creates height-based snow detail on mask channel 0
	void set_detail_render_order();		// Configures correct layering order

	Unigine::ObjectLandscapeTerrainPtr terrain;
	Unigine::TexturePtr grass_texture;
	Unigine::TexturePtr stone_texture;
	Unigine::MaterialPtr grass_material;
	Unigine::MaterialPtr stone_material;
	Unigine::MaterialPtr snow_material;
	bool created{false};	// Prevents duplicate detail creation

	void init_gui();
	// Displays texture preview for a detail type
	void add_detail_properties(const Unigine::WidgetPtr &parent, const char *title,
		decltype(grass_texture_param) &texture);
	void shutdown_gui();

	SampleDescriptionWindow sample_description_window;

	void add_details_button_callback();	// Handles Create button click
};