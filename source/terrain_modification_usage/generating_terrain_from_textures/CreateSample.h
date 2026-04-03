// Demonstrates procedural LandscapeTerrain creation from source images.
// Albedo, height, and mask textures are combined into a landscape map file (.lmap).
// The map is then applied to an ObjectLandscapeTerrain via LandscapeLayerMap.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

// Creates a landscape terrain from grid of source images at runtime.
class CreateSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CreateSample, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(IVec2, resolution_param, "Resolution");	// Total landscape map resolution in pixels
	PROP_PARAM(IVec2, grid_size_param, "Grid Size");	// Number of tiles in X and Y directions

	PROP_ARRAY(File, albedo_images_param, "Albedo Images");			// Color/diffuse textures for each tile
	PROP_ARRAY(File, height_images_param, "Height Images");			// Heightmap images for each tile
	PROP_ARRAY(File, grass_mask_images_param, "Grass Mask Images");	// Grass detail mask for each tile
	PROP_ARRAY(File, stone_mask_images_param, "Stone Mask Images");	// Stone detail mask for each tile

	PROP_PARAM(Vec2, terrain_size, Unigine::Math::vec2(1000.0f, 1000.0f));	// World size of the terrain in units
	PROP_PARAM(Float, terrain_height_scale, 150.0f);						// Vertical scale factor for heightmap

private:
	void init();
	void shutdown();

	void create_landscape_map();		// Generates the .lmap file from source images
	void create_landscape_objects();	// Creates terrain node and attaches the layer map

	// Callback invoked for each tile during map creation
	void create_callback(const Unigine::LandscapeMapFileCreatorPtr &creator,
		const Unigine::LandscapeImagesPtr &images, int x, int y);


	bool created{false};	// Prevents duplicate terrain creation

	void init_gui();
	// Displays image thumbnails in a grid layout
	void add_image_grid(const Unigine::WidgetPtr &parent, const char *title,
		decltype(albedo_images_param) &images);
	void shutdown_gui();
	void create_button_callback();	// Handles Create button click

private:
	SampleDescriptionWindow sample_description_window;
};
