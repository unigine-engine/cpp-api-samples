#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class DetailsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DetailsSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, terrain_param, "Terrain");
	PROP_PARAM(File, grass_texture_param, "Grass Texture");
	PROP_PARAM(File, stone_texture_param, "Stone Texture");

private:
	void init();
	void shutdown();

	void create_detail_grass();
	void create_detail_stone();
	void create_detail_snow_cap();
	void set_detail_render_order();

	Unigine::ObjectLandscapeTerrainPtr terrain;
	Unigine::TexturePtr grass_texture;
	Unigine::TexturePtr stone_texture;
	Unigine::MaterialPtr grass_material;
	Unigine::MaterialPtr stone_material;
	Unigine::MaterialPtr snow_material;
	bool created{false};

	// Gui stuff
	void init_gui();
	void add_detail_properties(const Unigine::WidgetPtr &parent, const char *title,
		decltype(grass_texture_param) &texture);
	void shutdown_gui();

	SampleDescriptionWindow sample_description_window;

	void add_details_button_callback();
};