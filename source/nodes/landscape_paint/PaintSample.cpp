#include "PaintSample.h"

REGISTER_COMPONENT(PaintSample);

using namespace Unigine;
using namespace Math;

void PaintSample::init()
{
	saved_mouse_handle = ControlsApp::getMouseHandle();
	ControlsApp::setMouseHandle(Input::MOUSE_HANDLE_USER);

	albedo_painter = getComponent<LandscapeAlbedoPainter>(albedo_painter_param);
	height_painter = getComponent<LandscapeHeightPainter>(height_painter_param);
	height_to_albedo = getComponent<LandscapeHeightToAlbedo>(height_to_albedo_param);

	player = static_ptr_cast<PlayerSpectator>(player_param.get());
	landscape_fetch = LandscapeFetch::create();

	auto guid = FileSystem::getGUID(
		FileSystem::resolvePartialVirtualPath("decal_brush_base.basemat"));
	if (!guid.isValid())
	{
		Log::warning("PaintSample::init(): can not find \"decal_brush_base.basemat\" material\n");
		return;
	}

	brush_decal_material = Materials::findMaterialByFileGUID(guid)->inherit();
	brush_decal = DecalOrtho::create(); // 100.0f, 1.0f, 1.0f, "decal_base"
	brush_decal->setRadius(100.0f);
	brush_decal->setWidth(1.0f);
	brush_decal->setHeight(1.0f);
	brush_decal->setMaterial(brush_decal_material);

	init_resources();
	init_gui();

	landscape_layer_map = static_ptr_cast<LandscapeLayerMap>(landscape_layer_map_param.get());
	albedo_painter->setTarget(landscape_layer_map);
	height_painter->setTarget(landscape_layer_map);
	height_to_albedo->setTarget(landscape_layer_map);

	set_mode(mode);
	set_brush_texture(brush_textures[0]);
	set_brush_mask(brush_masks[0]);
	set_gradient(gradients[0]);
	set_brush_size(brush_size);
	set_brush_spacing(brush_spacing);
	set_brush_angle(brush_angle);
	set_brush_height(brush_height);
	set_brush_height_scale(brush_height_scale);
	set_brush_opacity(brush_opacity);
	set_brush_color(brush_color);
	set_height_blend_mode(blend_mode);
	set_min_height(min_height);
	set_max_height(max_height);
}

void PaintSample::init_resources()
{
	for (int i = 0; i < brush_textures_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(brush_textures_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		brush_textures.push_back(texture);

		image->resize(32, 32);
		brush_textures_icons.push_back(image);
	}

	for (int i = 0; i < brush_masks_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(brush_masks_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		brush_masks.push_back(texture);

		image->resize(32, 32);
		brush_masks_icons.push_back(image);
	}

	for (int i = 0; i < gradients_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(gradients_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		gradients.push_back(texture);

		image->resize(32, 128);
		gradiends_icons.push_back(image);
	}
}

void PaintSample::update()
{
	if (window->getMouseX() >= 0 && window->getMouseX() < window->getWidth() && window->getMouseY() >= 0 && window->getMouseY() < window->getHeight())
	{
		return;
	}

	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT))
	{
		ivec2 delta = Input::getMouseDeltaPosition();
		player->setPhiAngle(player->getPhiAngle() + delta.x);
		player->setThetaAngle(player->getThetaAngle() + delta.y);
	}

	if (Input::isKeyPressed(Input::KEY_LEFT_SHIFT) || Input::isKeyPressed(Input::KEY_RIGHT_SHIFT))
		set_brush_angle(brush_angle + Input::getMouseWheel() * 5);
	else if (Input::isKeyPressed(Input::KEY_LEFT_CTRL) || Input::isKeyPressed(Input::KEY_RIGHT_CTRL))
		set_brush_size(brush_size + Input::getMouseWheel() * 0.5f);


	if (mode == Mode::AlbedoPainter || mode == Mode::HeightPainter)
	{
		const auto mouse_coord = Input::getMousePosition();
		bool fetched = landscape_fetch->intersectionForce(player->getPosition(),
			player->getPosition() + Vec3(player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y)) * 10000.0);

		if (fetched)
		{
			brush_decal->setPosition(landscape_fetch->getPosition() + Vec3(0.0, 0.0, 50.0));

			if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
			{
				switch (mode)
				{
					case Mode::AlbedoPainter:
						albedo_painter->paintAt(landscape_fetch->getPosition());
						break;
					case Mode::HeightPainter:
						height_painter->paintAt(landscape_fetch->getPosition());
						break;
					default: break;
				}
			}
		}
	}
}

void PaintSample::shutdown()
{
	albedo_painter->disable();
	height_painter->disable();
	height_to_albedo->disable();

	ControlsApp::setMouseHandle(saved_mouse_handle);

	shutdown_gui();
}

void PaintSample::set_mode(Mode in_mode)
{
	mode = in_mode;

	switch (mode)
	{
		case PaintSample::Mode::AlbedoPainter:
			albedo_painter->enable();
			height_painter->disable();
			height_to_albedo->disable();
			brush_decal->setEnabled(true);
			tab_box->setCurrentTab(0);
			break;
		case PaintSample::Mode::HeightPainter:
			albedo_painter->disable();
			height_painter->enable();
			height_to_albedo->disable();
			brush_decal->setEnabled(true);
			tab_box->setCurrentTab(1);
			break;
		case PaintSample::Mode::HeightToAlbedo:
			albedo_painter->disable();
			height_painter->disable();
			height_to_albedo->enable();
			brush_decal->setEnabled(false);
			tab_box->setCurrentTab(2);
			break;
		default:
			albedo_painter->disable();
			height_painter->disable();
			height_to_albedo->disable();
			brush_decal->setEnabled(false);
			break;
	}
}

void PaintSample::set_brush_texture(const Unigine::TexturePtr &in_brush_texture)
{
	brush_texture = in_brush_texture;

	albedo_painter->setBrushTexture(in_brush_texture);
	height_painter->setBrushTexture(in_brush_texture);

	brush_decal_material->setTexture("albedo", brush_texture);
}

void PaintSample::set_brush_mask(const Unigine::TexturePtr &in_brush_mask)
{
	brush_mask = in_brush_mask;

	albedo_painter->setBrushMask(in_brush_mask);
	height_painter->setBrushMask(in_brush_mask);

	brush_decal_material->setTexture("mask", brush_mask);
}

void PaintSample::set_gradient(const Unigine::TexturePtr &in_gradient)
{
	gradient = in_gradient;

	height_to_albedo->setGradient(gradient);
}

void PaintSample::set_brush_size(float in_brush_size, bool update_ui)
{
	brush_size = max(in_brush_size, 1.0f);

	albedo_painter->setBrushSize(brush_size);
	height_painter->setBrushSize(brush_size);

	brush_decal->setWidth(brush_size);
	brush_decal->setHeight(brush_size);

	if (update_ui)
	{
		albedo_brush_size_edit_line_callback.disconnect();
		albedo_brush_size_edit_line->setText(String::ftoa(brush_size));
		albedo_brush_size_edit_line->getEventChanged().connect(
			albedo_brush_size_edit_line_callback, this, &PaintSample::brush_size_edit_line_callback);

		height_brush_size_edit_line_callback.disconnect();
		height_brush_size_edit_line->setText(String::ftoa(brush_size));
		height_brush_size_edit_line->getEventChanged().connect(
			height_brush_size_edit_line_callback, this, &PaintSample::brush_size_edit_line_callback);
	}
}

void PaintSample::set_brush_spacing(float in_brush_spacing, bool update_ui)
{
	brush_spacing = max(in_brush_spacing, 0.0f);

	albedo_painter->setBrushSpacing(brush_spacing);
	height_painter->setBrushSpacing(brush_spacing);

	if (update_ui)
	{
		albedo_brush_spacing_edit_line_callback.disconnect();
		albedo_brush_spacing_edit_line->setText(String::ftoa(brush_spacing));
		albedo_brush_spacing_edit_line->getEventChanged().connect(
			albedo_brush_spacing_edit_line_callback, this, &PaintSample::brush_spacing_edit_line_callback);

		height_brush_spacing_edit_line_callback.disconnect();
		height_brush_spacing_edit_line->setText(String::ftoa(brush_spacing));
		height_brush_spacing_edit_line->getEventChanged().connect(
			height_brush_spacing_edit_line_callback, this, &PaintSample::brush_spacing_edit_line_callback);
	}
}

void PaintSample::set_brush_angle(float in_brush_angle, bool update_ui)
{
	brush_angle = in_brush_angle;

	albedo_painter->setBrushAngle(brush_angle);
	height_painter->setBrushAngle(brush_angle);

	brush_decal->setRotation(quat(0.0f, 0.0f, -brush_angle), true);

	if (update_ui)
	{
		albedo_brush_angle_edit_line_callback.disconnect();
		albedo_brush_angle_edit_line->setText(String::ftoa(brush_angle));
		albedo_brush_angle_edit_line->getEventChanged().connect(
			albedo_brush_angle_edit_line_callback, this, &PaintSample::brush_angle_edit_line_callback);

		height_brush_angle_edit_line_callback.disconnect();
		height_brush_angle_edit_line->setText(String::ftoa(brush_angle));
		height_brush_angle_edit_line->getEventChanged().connect(
			height_brush_angle_edit_line_callback, this, &PaintSample::brush_angle_edit_line_callback);
	}
}

void PaintSample::set_brush_height(float in_brush_height, bool update_ui)
{
	brush_height = in_brush_height;

	height_painter->setBrushHeight(in_brush_height);

	if (update_ui)
	{
		height_brush_height_edit_line_callback.disconnect();
		height_brush_height_edit_line->setText(String::ftoa(brush_height));
		height_brush_height_edit_line->getEventChanged().connect(
			height_brush_height_edit_line_callback, this, &PaintSample::brush_height_edit_line_callback);
	}
}

void PaintSample::set_brush_height_scale(float in_brush_height_scale, bool update_ui)
{
	brush_height_scale = in_brush_height_scale;

	height_painter->setBrushHeightScale(in_brush_height_scale);

	if (update_ui)
	{
		height_brush_height_scale_edit_line_callback.disconnect();
		height_brush_height_scale_edit_line->setText(String::ftoa(brush_height_scale));
		height_brush_height_scale_edit_line->getEventChanged().connect(
			height_brush_height_scale_edit_line_callback, this, &PaintSample::brush_height_scale_edit_line_callback);
	}
}

void PaintSample::set_brush_opacity(float in_brush_opacity, bool update_ui)
{
	brush_opacity = clamp(in_brush_opacity, 0.0f, 1.0f);

	albedo_painter->setBrushOpacity(brush_opacity);
	height_painter->setBrushOpacity(brush_opacity);

	if (update_ui)
	{
		albedo_brush_opacity_edit_line_callback.disconnect();
		albedo_brush_opacity_edit_line->setText(String::ftoa(brush_opacity));
		albedo_brush_opacity_edit_line->getEventChanged().connect(
			albedo_brush_opacity_edit_line_callback, this, &PaintSample::brush_opacity_edit_line_callback);

		height_brush_opacity_edit_line_callback.disconnect();
		height_brush_opacity_edit_line->setText(String::ftoa(brush_opacity));
		height_brush_opacity_edit_line->getEventChanged().connect(
			height_brush_opacity_edit_line_callback, this, &PaintSample::brush_opacity_edit_line_callback);
	}
}

void PaintSample::set_brush_color(const Unigine::Math::vec3 &in_brush_color, bool update_ui)
{
	brush_color = clamp(in_brush_color, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	albedo_painter->setBrushColor(brush_color);

	brush_decal_material->setParameterFloat4("albedo", {brush_color, 1.0f});

	if (update_ui)
	{
		albedo_brush_color_r_edit_line_callback.disconnect();
		albedo_brush_color_r_edit_line->setText(String::itoa(ftoi(brush_color.x * 255.0f)));
		albedo_brush_color_r_edit_line->getEventChanged().connect(
			albedo_brush_color_r_edit_line_callback, this, &PaintSample::brush_color_r_edit_line_callback);

		albedo_brush_color_g_edit_line_callback.disconnect();
		albedo_brush_color_g_edit_line->setText(String::itoa(ftoi(brush_color.y * 255.0f)));
		albedo_brush_color_g_edit_line->getEventChanged().connect(
			albedo_brush_color_g_edit_line_callback, this, &PaintSample::brush_color_g_edit_line_callback);

		albedo_brush_color_b_edit_line_callback.disconnect();
		albedo_brush_color_b_edit_line->setText(String::itoa(ftoi(brush_color.z * 255.0f)));
		albedo_brush_color_b_edit_line->getEventChanged().connect(
			albedo_brush_color_b_edit_line_callback, this, &PaintSample::brush_color_b_edit_line_callback);
	}
}

void PaintSample::set_height_blend_mode(BlendMode in_blend_mode, bool update_ui)
{
	blend_mode = in_blend_mode;

	height_painter->setBlendMode(in_blend_mode);

	if (update_ui)
	{
		height_blend_mode_combo_box_callback.disconnect();
		switch (blend_mode)
		{
			case BlendMode::Alpha: height_blend_mode_combo_box->setCurrentItem(0); break;
			case BlendMode::Additive: height_blend_mode_combo_box->setCurrentItem(1); break;
		}
		height_blend_mode_combo_box->getEventChanged().connect(
			height_blend_mode_combo_box_callback, this, &PaintSample::blend_mode_combo_box_callback);
	}
}

void PaintSample::set_min_height(float in_min_height, bool update_ui)
{
	min_height = in_min_height;

	height_to_albedo->setMinHeight(min_height);

	if (update_ui)
	{
		min_height_edit_line_callback_.disconnect();
		min_height_edit_line->setText(String::ftoa(min_height));
		min_height_edit_line->getEventChanged().connect(
			min_height_edit_line_callback_, this, &PaintSample::min_height_edit_line_callback);
	}
}

void PaintSample::set_max_height(float in_max_height, bool update_ui)
{
	max_height = in_max_height;

	height_to_albedo->setMaxHeight(max_height);

	if (update_ui)
	{
		max_height_edit_line_callback_.disconnect();
		max_height_edit_line->setText(String::ftoa(max_height));
		max_height_edit_line->getEventChanged().connect(
			max_height_edit_line_callback_, this, &PaintSample::max_height_edit_line_callback);
	}
}

void PaintSample::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	window = sample_description_window.getWindow();

	tab_box = WidgetTabBox::create();
	tab_box->getEventChanged().connect(this, &PaintSample::tab_box_callback);
	tab_box->addTab("Albedo Painter");

	auto hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create("Texture");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	auto gridbox = WidgetGridBox::create(6);
	tab_box->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_textures.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_textures_icons[i]);
		button->getEventClicked().connect(this, &PaintSample::brush_texture_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Mask");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	gridbox = WidgetGridBox::create(6);
	tab_box->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_masks.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_masks_icons[i]);
		button->getEventClicked().connect(this, &PaintSample::brush_mask_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Size");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_size_edit_line = WidgetEditLine::create();
	albedo_brush_size_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(albedo_brush_size_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Spacing");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_spacing_edit_line = WidgetEditLine::create();
	albedo_brush_spacing_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(albedo_brush_spacing_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Angle");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_angle_edit_line = WidgetEditLine::create();
	albedo_brush_angle_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(albedo_brush_angle_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Opacity");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_opacity_edit_line = WidgetEditLine::create();
	albedo_brush_opacity_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(albedo_brush_opacity_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Color");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("R");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_color_r_edit_line = WidgetEditLine::create();
	albedo_brush_color_r_edit_line->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(albedo_brush_color_r_edit_line, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("G");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_color_g_edit_line = WidgetEditLine::create();
	albedo_brush_color_g_edit_line->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(albedo_brush_color_g_edit_line, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("B");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	albedo_brush_color_b_edit_line = WidgetEditLine::create();
	albedo_brush_color_b_edit_line->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(albedo_brush_color_b_edit_line, Gui::ALIGN_LEFT);


	tab_box->addTab("Height Painter");

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Texture");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	gridbox = WidgetGridBox::create(6);
	tab_box->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_textures.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_textures_icons[i]);
		button->getEventClicked().connect(this, &PaintSample::brush_texture_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Mask");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	gridbox = WidgetGridBox::create(6);
	tab_box->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_masks.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_masks_icons[i]);
		button->getEventClicked().connect(this, &PaintSample::brush_mask_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Size");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_size_edit_line = WidgetEditLine::create();
	height_brush_size_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_size_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Spacing");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_spacing_edit_line = WidgetEditLine::create();
	height_brush_spacing_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_spacing_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Angle");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_angle_edit_line = WidgetEditLine::create();
	height_brush_angle_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_angle_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Opacity");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_opacity_edit_line = WidgetEditLine::create();
	height_brush_opacity_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_opacity_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Height");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_height_edit_line = WidgetEditLine::create();
	height_brush_height_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_height_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Height Scale");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_brush_height_scale_edit_line = WidgetEditLine::create();
	height_brush_height_scale_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(height_brush_height_scale_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Blend Mode");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	height_blend_mode_combo_box = WidgetComboBox::create();
	height_blend_mode_combo_box->addItem("Alpha");
	height_blend_mode_combo_box->addItem("Additive");
	hbox->addChild(height_blend_mode_combo_box);


	tab_box->addTab("Height To Albedo");

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Min Height");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	min_height_edit_line = WidgetEditLine::create();
	min_height_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(min_height_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Max Height");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	max_height_edit_line = WidgetEditLine::create();
	max_height_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(max_height_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	tab_box->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Gradient");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	gridbox = WidgetGridBox::create(6);
	tab_box->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < gradients.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(gradiends_icons[i]);
		button->getEventClicked().connect(this, &PaintSample::gradient_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	parameters->addChild(tab_box);
}

void PaintSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

void PaintSample::tab_box_callback()
{
	switch (tab_box->getCurrentTab())
	{
		case 0: set_mode(Mode::AlbedoPainter); break;
		case 1: set_mode(Mode::HeightPainter); break;
		case 2: set_mode(Mode::HeightToAlbedo); break;
	}
}

void PaintSample::brush_texture_button_callback(int index)
{
	set_brush_texture(brush_textures[index]);
}

void PaintSample::brush_mask_button_callback(int index)
{
	set_brush_mask(brush_masks[index]);
}

void PaintSample::gradient_button_callback(int index)
{
	set_gradient(gradients[index]);
	height_to_albedo->run();
}

void PaintSample::brush_size_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_size(value, true);
}

void PaintSample::brush_spacing_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_spacing(value, false);
}

void PaintSample::brush_angle_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_angle(value, false);
}

void PaintSample::brush_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_height(value, false);
}

void PaintSample::brush_height_scale_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_height_scale(value, false);
}

void PaintSample::brush_opacity_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_opacity(value, false);
}

void PaintSample::brush_color_r_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({value, brush_color.y, brush_color.z}, false);
}

void PaintSample::brush_color_g_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({brush_color.x, value, brush_color.z}, false);
}

void PaintSample::brush_color_b_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({brush_color.x, brush_color.y, value}, false);
}

void PaintSample::min_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_min_height(value, false);
}

void PaintSample::max_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_max_height(value, false);
}

void PaintSample::blend_mode_combo_box_callback(const Unigine::WidgetPtr &widget)
{
	switch (static_ptr_cast<WidgetComboBox>(widget)->getCurrentItem())
	{
		case 0: set_height_blend_mode(BlendMode::Alpha); break;
		case 1: set_height_blend_mode(BlendMode::Additive); break;
	}
}

void PaintSample::height_to_albedo_run_button_callback()
{
	height_to_albedo->run();
}
