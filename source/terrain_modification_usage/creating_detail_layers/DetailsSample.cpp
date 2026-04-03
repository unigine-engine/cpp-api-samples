// Demonstrates runtime creation of terrain detail layers on ObjectLandscapeTerrain.
// Detail materials for grass, stone, and snow are created and applied to mask channels.
// Render order is configured to layer details correctly (stone, grass, then snow caps).

#include "DetailsSample.h"

REGISTER_COMPONENT(DetailsSample);

using namespace Unigine;
using namespace Math;

// GUI is initialized; terrain reference is obtained from property.
void DetailsSample::init()
{
	init_gui();

	// Terrain node is cast to ObjectLandscapeTerrain for detail mask access
	terrain = static_ptr_cast<ObjectLandscapeTerrain>(terrain_param.get());
}

// GUI resources are released.
void DetailsSample::shutdown()
{
	shutdown_gui();
}

// Grass detail is created with triplanar mapping on mask channel 1.
void DetailsSample::create_detail_grass()
{
	// Texture is loaded from file path property
	grass_texture = Texture::create();
	grass_texture->load(grass_texture_param.get());

	// Material is inherited from base terrain detail material
	grass_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	grass_material->setState("triplanar", 1);	// Triplanar projection avoids UV stretching on slopes
	grass_material->setParameterFloat("size", 5.0f);
	grass_material->setParameterFloat("albedo_opacity", 0.8f);
	grass_material->setParameterFloat4("albedo", {0.8f, 0.8f, 0.6f, 1.0f});
	grass_material->setTexture("albedo", grass_texture);
	grass_material->setParameterFloat("height_opacity", 0.0f);	// No height displacement

	// Detail is added to mask channel 1
	auto grass_mask = terrain->getDetailMask(1);
	grass_mask->setName("Grass Mask");

	auto grass_detail = grass_mask->addDetail();
	grass_detail->setName("Grass Detail");
	grass_detail->setMaterial(grass_material);
}

// Stone detail is created with triplanar mapping on mask channel 2.
void DetailsSample::create_detail_stone()
{
	stone_texture = Texture::create();
	stone_texture->load(stone_texture_param.get());

	// Material settings similar to grass but with different tint and scale
	stone_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	stone_material->setState("triplanar", 1);
	stone_material->setParameterFloat("size", 4.0f);
	stone_material->setParameterFloat("albedo_opacity", 0.9f);
	stone_material->setParameterFloat4("albedo", {0.6f, 0.6f, 0.5f, 1.0f});
	stone_material->setTexture("albedo", stone_texture);
	stone_material->setParameterFloat("height_opacity", 0.0f);

	// Detail is added to mask channel 2
	auto stone_mask = terrain->getDetailMask(2);
	stone_mask->setName("Stone Mask");

	auto stone_detail = stone_mask->addDetail();
	stone_detail->setName("Stone Detail");
	stone_detail->setMaterial(stone_material);
}

// Snow cap detail appears only above certain terrain height on mask channel 0.
void DetailsSample::create_detail_snow_cap()
{
	// White material uses default albedo color (no texture)
	snow_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	snow_material->setParameterFloat("height_opacity", 0.0f);

	// Detail is added to mask channel 0 (white/base mask)
	auto white_mask = terrain->getDetailMask(0);
	white_mask->setName("White Mask");

	auto stone_detail = white_mask->addDetail();
	stone_detail->setName("Snow Cap Detail");
	stone_detail->setMaterial(snow_material);
	// Snow appears only above 100m with 30m fade transition
	stone_detail->setMinVisibleHeight(100.0f);
	stone_detail->setMinFadeHeight(30.0f);
}

// Render order is swapped so snow renders last (on top of other details).
void DetailsSample::set_detail_render_order()
{
	// Desired render order:
	// 1. stone (renders first, can be covered)
	// 2. grass
	// 3. snow caps (renders last, covers others at high altitude)

	auto white_mask = terrain->getDetailMask(0);
	auto stone_mask = terrain->getDetailMask(2);
	// Swap moves white mask (snow) to render after stone mask
	white_mask->swapRenderOrder(stone_mask);
}

// Sample window is created with Create button and texture previews.
void DetailsSample::init_gui()
{
	sample_description_window.createWindow();

	auto window = sample_description_window.getWindow();
	auto create_details_button = WidgetButton::create("Create details");
	window->addChild(create_details_button);

	create_details_button->getEventClicked().connect(this, &DetailsSample::add_details_button_callback);

	// Texture previews are added for grass and stone
	add_detail_properties(window, "Grass", grass_texture_param);
	add_detail_properties(window, "Stone", stone_texture_param);
}

// Group box with texture thumbnail is added for the specified detail type.
void DetailsSample::add_detail_properties(const Unigine::WidgetPtr &parent, const char *title,
	decltype(grass_texture_param) &texture)
{
	auto group_box = WidgetGroupBox::create(title, 4, 0);
	parent->addChild(group_box, Gui::ALIGN_LEFT);

	auto hbox = WidgetHBox::create();
	group_box->addChild(hbox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create("Texture");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);

	// Image is loaded and resized for thumbnail display
	auto image = Image::create(texture.get());
	image->resize(64, 64);
	auto sprite = WidgetSprite::create();
	sprite->setImage(image);
	hbox->addChild(sprite, Gui::ALIGN_LEFT);
}

// Sample window is destroyed.
void DetailsSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

// All detail layers are created when button is clicked; duplicate creation is prevented.
void DetailsSample::add_details_button_callback()
{
	if (created)
		return;

	create_detail_grass();
	create_detail_stone();
	create_detail_snow_cap();
	set_detail_render_order();
	created = true;
}
