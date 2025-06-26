#include "DetailsSample.h"

REGISTER_COMPONENT(DetailsSample);

using namespace Unigine;
using namespace Math;

void DetailsSample::init()
{
	init_gui();

	terrain = static_ptr_cast<ObjectLandscapeTerrain>(terrain_param.get());
}

void DetailsSample::shutdown()
{
	shutdown_gui();
}

void DetailsSample::create_detail_grass()
{
	grass_texture = Texture::create();
	grass_texture->load(grass_texture_param.get());

	grass_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	grass_material->setState("triplanar", 1);
	grass_material->setParameterFloat("size", 5.0f);
	grass_material->setParameterFloat("albedo_opacity", 0.8f);
	grass_material->setParameterFloat4("albedo", {0.8f, 0.8f, 0.6f, 1.0f});
	grass_material->setTexture("albedo", grass_texture);
	grass_material->setParameterFloat("height_opacity", 0.0f);


	auto grass_mask = terrain->getDetailMask(1);
	grass_mask->setName("Grass Mask");

	auto grass_detail = grass_mask->addDetail();
	grass_detail->setName("Grass Detail");
	grass_detail->setMaterial(grass_material);
}

void DetailsSample::create_detail_stone()
{
	stone_texture = Texture::create();
	stone_texture->load(stone_texture_param.get());

	stone_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	stone_material->setState("triplanar", 1);
	stone_material->setParameterFloat("size", 4.0f);
	stone_material->setParameterFloat("albedo_opacity", 0.9f);
	stone_material->setParameterFloat4("albedo", {0.6f, 0.6f, 0.5f, 1.0f});
	stone_material->setTexture("albedo", stone_texture);
	stone_material->setParameterFloat("height_opacity", 0.0f);

	auto stone_mask = terrain->getDetailMask(2);
	stone_mask->setName("Stone Mask");

	auto stone_detail = stone_mask->addDetail();
	stone_detail->setName("Stone Detail");
	stone_detail->setMaterial(stone_material);
}

void DetailsSample::create_detail_snow_cap()
{
	snow_material
		= Materials::findManualMaterial("Unigine::landscape_terrain_detail_base")->inherit();
	snow_material->setParameterFloat("height_opacity", 0.0f);

	auto white_mask = terrain->getDetailMask(0);
	white_mask->setName("White Mask");

	auto stone_detail = white_mask->addDetail();
	stone_detail->setName("Snow Cap Detail");
	stone_detail->setMaterial(snow_material);
	stone_detail->setMinVisibleHeight(100.0f);
	stone_detail->setMinFadeHeight(30.0f);
}

void DetailsSample::set_detail_render_order()
{
	// desired render order:
	// 1. stone
	// 2. grass
	// 3. snow caps

	auto white_mask = terrain->getDetailMask(0);
	auto stone_mask = terrain->getDetailMask(2);
	white_mask->swapRenderOrder(stone_mask);
}

void DetailsSample::init_gui()
{
	sample_description_window.createWindow();

	auto window = sample_description_window.getWindow();
	auto create_details_button = WidgetButton::create("Create details");
	window->addChild(create_details_button);

	create_details_button->getEventClicked().connect(this, &DetailsSample::add_details_button_callback);

	add_detail_properties(window, "Grass", grass_texture_param);
	add_detail_properties(window, "Stone", stone_texture_param);
}

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
	auto image = Image::create(texture.get());
	image->resize(64, 64);
	auto sprite = WidgetSprite::create();
	sprite->setImage(image);
	hbox->addChild(sprite, Gui::ALIGN_LEFT);
}


void DetailsSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

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
