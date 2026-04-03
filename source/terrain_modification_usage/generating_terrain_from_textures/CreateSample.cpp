// Demonstrates procedural LandscapeTerrain creation from source images.
// Albedo, height, and mask textures are combined into a landscape map file (.lmap).
// The map is then applied to an ObjectLandscapeTerrain via LandscapeLayerMap.

#include "CreateSample.h"

REGISTER_COMPONENT(CreateSample);

using namespace Unigine;
using namespace Math;

// GUI is initialized; source image arrays are validated against grid dimensions.
void CreateSample::init()
{
	init_gui();

	// Expected tile count is calculated from grid dimensions
	int image_count = grid_size_param.get().x * grid_size_param.get().y;
	if (albedo_images_param.size() != image_count
		|| height_images_param.size() != image_count
		|| grass_mask_images_param.size() != image_count
		|| stone_mask_images_param.size() != image_count)
	{
		Log::error("Expected %i terrain images\n", image_count);
		return;
	}
}

// GUI resources are released.
void CreateSample::shutdown()
{
	shutdown_gui();
}

// Landscape map file is created from source images; layer settings are configured.
void CreateSample::create_landscape_map()
{
	// Map file creator is configured with resolution and grid layout
	auto map_file_creator = LandscapeMapFileCreator::create();
	map_file_creator->setResolution(resolution_param.get());
	map_file_creator->setGrid(grid_size_param.get());
	map_file_creator->getEventCreate().connect(this, &CreateSample::create_callback);
	map_file_creator->setPath("landscape_map.gen.lmap");
	map_file_creator->run();

	// Map file settings are loaded to configure layer blending modes
	auto map_file_settings = LandscapeMapFileSettings::create();
	map_file_settings->load(FileSystem::getGUID("landscape_map.gen.lmap"));

	// Albedo layer is enabled with alpha blending
	map_file_settings->setEnabledAlbedo(true);
	map_file_settings->setAlbedoBlending(Unigine::Landscape::BLENDING_MODE::ALPHA_BLEND);
	map_file_settings->setEnabledOpacityAlbedo(false);

	// Height layer is enabled with alpha blending
	map_file_settings->setEnabledHeight(true);
	map_file_settings->setHeightBlending(Unigine::Landscape::BLENDING_MODE::ALPHA_BLEND);
	map_file_settings->setEnabledOpacityHeight(false);

	// Mask channel 0 (base) is enabled
	map_file_settings->setEnabledMask(0, true);
	map_file_settings->setMaskBlending(0, Unigine::Landscape::BLENDING_MODE::ALPHA_BLEND);
	map_file_settings->setEnabledOpacityMask(0, false);

	// Mask channel 1 (grass) is enabled
	map_file_settings->setEnabledMask(1, true);
	map_file_settings->setMaskBlending(1, Unigine::Landscape::BLENDING_MODE::ALPHA_BLEND);
	map_file_settings->setEnabledOpacityMask(1, false);

	// Mask channel 2 (stone) is enabled
	map_file_settings->setEnabledMask(2, true);
	map_file_settings->setMaskBlending(2, Unigine::Landscape::BLENDING_MODE::ALPHA_BLEND);
	map_file_settings->setEnabledOpacityMask(2, false);

	map_file_settings->apply();
}

// Terrain object is created and layer map is attached with configured parameters.
void CreateSample::create_landscape_objects()
{
	// Terrain object is created and set as active
	auto terrain = ObjectLandscapeTerrain::create();
	terrain->setActiveTerrain(true);

	// Layer map is created as child of terrain
	auto layer_map = LandscapeLayerMap::create();
	layer_map->setParent(terrain);
	layer_map->setPath("landscape_map.gen.lmap");
	layer_map->setHeightScale(terrain_height_scale);
	layer_map->setSize(Unigine::Math::Vec2(terrain_size));
	layer_map->setPosition(Vec3_zero);
}

// Tile data is loaded from source images and copied into the landscape image buffers.
void CreateSample::create_callback(const LandscapeMapFileCreatorPtr &creator, const LandscapeImagesPtr &images, int x, int y)
{
	// Linear tile index is calculated from grid coordinates
	int index = y * grid_size_param.get().x + x;
	ivec2 tile_resolution = resolution_param.get() / grid_size_param.get();

	// Source images are loaded for current tile
	auto source_albedo = Image::create(albedo_images_param.get(index).get());
	auto source_height = Image::create(height_images_param.get(index).get());
	auto source_grass_mask = Image::create(grass_mask_images_param.get(index).get());
	auto source_stone_mask = Image::create(stone_mask_images_param.get(index).get());

	// Albedo image is validated and copied to landscape buffer
	if (source_albedo->getWidth() == tile_resolution.x && source_albedo->getHeight() == tile_resolution.y)
	{
		auto albedo = images->getAlbedo();
		albedo->create2D(source_albedo->getWidth(), source_albedo->getHeight(), source_albedo->getFormat());
		albedo->copy(source_albedo, 0, 0, 0, 0, source_albedo->getWidth(), source_albedo->getHeight());
	}
	else
		Log::error("Incorrect albedo tile image size\n");

	// Height image is validated and copied to landscape buffer
	if (source_height->getWidth() == tile_resolution.x && source_height->getHeight() == tile_resolution.y)
	{
		auto height = images->getHeight();
		height->create2D(source_height->getWidth(), source_height->getHeight(), source_height->getFormat());
		height->copy(source_height, 0, 0, 0, 0, source_height->getWidth(), source_height->getHeight());
	}
	else
		Log::error("Incorrect height tile image size\n");

	// Mask image is created with RGBA channels for multiple detail layers
	auto masks = images->getMask(0);
	masks->create2D(source_albedo->getWidth(), source_albedo->getHeight(), Image::FORMAT_RGBA8);
	masks->setChannelInt(0, 255);	// Channel 0 is set to full opacity (base layer)

	// Grass mask is copied to channel 1
	if (source_grass_mask->getWidth() == tile_resolution.x && source_grass_mask->getHeight() == tile_resolution.y)
		masks->copy(source_grass_mask, 1, 0);
	else
		Log::error("Incorrect grass mask tile image size\n");

	// Stone mask is copied to channel 2
	if (source_stone_mask->getWidth() == tile_resolution.x && source_stone_mask->getHeight() == tile_resolution.y)
		masks->copy(source_stone_mask, 2, 0);
	else
		Log::error("Incorrect stone mask tile image size\n");
}

// Sample window is created with Create button and image preview grids.
void CreateSample::init_gui()
{
	sample_description_window.createWindow();

	auto window = sample_description_window.getWindow();

	// Create button triggers terrain generation
	auto button = WidgetButton::create("Create");
	button->setWidth(200);
	button->setHeight(20);
	button->getEventClicked().connect(this, &CreateSample::create_button_callback);
	window->addChild(button, Gui::ALIGN_LEFT);

	// Image grids display thumbnails of source textures
	add_image_grid(window, "Albedo", albedo_images_param);
	add_image_grid(window, "Height", height_images_param);
	add_image_grid(window, "Grass mask", grass_mask_images_param);
	add_image_grid(window, "Stone mask", stone_mask_images_param);


	window->arrange();
}

// Thumbnail grid is created from source images for visual preview.
void CreateSample::add_image_grid(const Unigine::WidgetPtr &parent, const char *title, decltype(albedo_images_param) &images)
{
	auto hbox = WidgetHBox::create(4, 4);
	parent->addChild(hbox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create(title);
	hbox->addChild(label, Gui::ALIGN_LEFT);

	// Grid widget arranges thumbnails in rows and columns matching tile layout
	auto grid = WidgetGridBox::create(grid_size_param.get().x, 4, 4);
	for (int i = 0; i < grid_size_param.get().y; ++i)
	{
		for (int j = 0; j < grid_size_param.get().x; ++j)
		{
			// Image is loaded and resized to thumbnail dimensions
			auto image = Image::create(images.get(grid_size_param.get().x * (grid_size_param.get().y - i - 1) + j).get());
			image->resize(64, 64);
			image->convertToFormat(Image::FORMAT_RGB8);
			image->flipY();
			auto sprite = WidgetSprite::create();
			sprite->setImage(image);
			grid->addChild(sprite);
		}
	}
	parent->addChild(grid, Gui::ALIGN_LEFT);
}


// Sample window is destroyed.
void CreateSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

// Terrain is generated when button is clicked; duplicate creation is prevented.
void CreateSample::create_button_callback()
{
	if (created)
		return;

	create_landscape_map();
	create_landscape_objects();
	created = true;
}
