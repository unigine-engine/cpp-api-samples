#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class CreateSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CreateSample, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(IVec2, resolution_param, "Resolution");
	PROP_PARAM(IVec2, grid_size_param, "Grid Size");

	PROP_ARRAY(File, albedo_images_param, "Albedo Images");
	PROP_ARRAY(File, height_images_param, "Height Images");
	PROP_ARRAY(File, grass_mask_images_param, "Grass Mask Images");
	PROP_ARRAY(File, stone_mask_images_param, "Stone Mask Images");

	PROP_PARAM(Vec2, terrain_size, Unigine::Math::vec2(1000.0f, 1000.0f));
	PROP_PARAM(Float, terrain_height_scale, 150.0f);

private:
	void init();
	void shutdown();

	void create_landscape_map();
	void create_landscape_objects();

	void create_callback(const Unigine::LandscapeMapFileCreatorPtr &creator,
		const Unigine::LandscapeImagesPtr &images, int x, int y);


	bool created{false};

	// Gui stuff
	void init_gui();
	void add_image_grid(const Unigine::WidgetPtr &parent, const char *title,
		decltype(albedo_images_param) &images);
	void shutdown_gui();
	void create_button_callback();

private:
	SampleDescriptionWindow sample_description_window;
};
