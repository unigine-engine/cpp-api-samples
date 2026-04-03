// Interactive GUI for landscape painting with albedo, height, and gradient modes.
// Coordinates multiple painter components and provides brush preview visualization.
// Supports real-time parameter editing with keyboard and mouse shortcuts.

#include "PaintSample.h"

REGISTER_COMPONENT(PaintSample);

using namespace Unigine;
using namespace Math;

// Components are obtained; brush preview decal is created; GUI is initialized.
void PaintSample::init()
{
	// Mouse handling mode is saved and switched to user mode
	saved_mouse_handle = ControlsApp::getMouseHandle();
	ControlsApp::setMouseHandle(Input::MOUSE_HANDLE_USER);

	// Painter components are obtained from referenced nodes
	albedo_painter = getComponent<LandscapeAlbedoPainter>(albedo_painter_param);
	height_painter = getComponent<LandscapeHeightPainter>(height_painter_param);
	height_to_albedo = getComponent<LandscapeHeightToAlbedo>(height_to_albedo_param);

	// Player camera and landscape fetch are initialized
	player = static_ptr_cast<PlayerSpectator>(player_param.get());
	landscape_fetch = LandscapeFetch::create();

	// Brush preview decal material is loaded
	auto guid = FileSystem::getGUID(
		FileSystem::resolvePartialVirtualPath("decal_brush_base.basemat"));
	if (!guid.isValid())
	{
		Log::warning("PaintSample::init(): can not find \"decal_brush_base.basemat\" material\n");
		return;
	}

	// Brush preview decal is created with inherited material
	brush_decal_material = Materials::findMaterialByFileGUID(guid)->inherit();
	brush_decal = DecalOrtho::create();
	brush_decal->setRadius(100.0f);
	brush_decal->setWidth(1.0f);
	brush_decal->setHeight(1.0f);
	brush_decal->setMaterial(brush_decal_material);

	// Resources and GUI are initialized
	init_resources();
	init_gui();

	// Landscape layer map is assigned to all painters
	landscape_layer_map = static_ptr_cast<LandscapeLayerMap>(landscape_layer_map_param.get());
	albedo_painter->setTarget(landscape_layer_map);
	height_painter->setTarget(landscape_layer_map);
	height_to_albedo->setTarget(landscape_layer_map);

	// Initial parameter values are applied to all painters
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

// Brush textures, masks, and gradients are loaded with thumbnails for GUI.
void PaintSample::init_resources()
{
	// Brush pattern textures are loaded and thumbnails are created
	for (int i = 0; i < brush_textures_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(brush_textures_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		brush_textures.push_back(texture);

		// Thumbnail is created by resizing image
		image->resize(32, 32);
		brush_textures_icons.push_back(image);
	}

	// Brush mask textures are loaded and thumbnails are created
	for (int i = 0; i < brush_masks_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(brush_masks_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		brush_masks.push_back(texture);

		// Thumbnail is created by resizing image
		image->resize(32, 32);
		brush_masks_icons.push_back(image);
	}

	// Gradient textures are loaded and thumbnails are created
	for (int i = 0; i < gradients_param.size(); ++i)
	{
		auto image = Image::create();
		image->load(gradients_param[i]);

		auto texture = Texture::create();
		texture->create(image);
		gradients.push_back(texture);

		// Gradient thumbnail preserves aspect ratio
		image->resize(32, 128);
		gradiends_icons.push_back(image);
	}
}

// Input is processed; brush preview is updated; painting is triggered on click.
void PaintSample::update()
{
	// Input is ignored when mouse is over window
	if (window->getMouseX() >= 0 && window->getMouseX() < window->getWidth() && window->getMouseY() >= 0 && window->getMouseY() < window->getHeight())
	{
		return;
	}

	// Camera rotation is controlled with right mouse button
	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT))
	{
		vec2 delta = vec2(Input::getMouseDeltaPosition()) * 0.2f;
		player->setPhiAngle(player->getPhiAngle() + delta.x);
		player->setThetaAngle(player->getThetaAngle() + delta.y);
	}

	// Brush angle is adjusted with Shift + mouse wheel
	if (Input::isKeyPressed(Input::KEY_LEFT_SHIFT) || Input::isKeyPressed(Input::KEY_RIGHT_SHIFT))
		set_brush_angle(brush_angle + Input::getMouseWheel() * 5);
	// Brush size is adjusted with Ctrl + mouse wheel
	else if (Input::isKeyPressed(Input::KEY_LEFT_CTRL) || Input::isKeyPressed(Input::KEY_RIGHT_CTRL))
		set_brush_size(brush_size + Input::getMouseWheel() * 0.5f);

	// Brush painting modes require terrain intersection
	if (mode == Mode::AlbedoPainter || mode == Mode::HeightPainter)
	{
		// Ray is cast from camera through mouse position
		const auto mouse_coord = Input::getMousePosition();
		bool fetched = landscape_fetch->intersectionForce(player->getPosition(),
			player->getPosition() + Vec3(player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y)) * 10000.0);

		if (fetched)
		{
			// Brush preview decal is positioned above terrain
			brush_decal->setPosition(landscape_fetch->getPosition() + Vec3(0.0, 0.0, 50.0));

			// Paint stroke is applied on left mouse button
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

// All painters are disabled; mouse handling is restored; GUI is released.
void PaintSample::shutdown()
{
	// All painter callbacks are disconnected
	albedo_painter->disable();
	height_painter->disable();
	height_to_albedo->disable();

	// Original mouse handling mode is restored
	ControlsApp::setMouseHandle(saved_mouse_handle);

	shutdown_gui();
}

// Active painting mode is switched; corresponding painter is enabled.
void PaintSample::set_mode(Mode in_mode)
{
	mode = in_mode;

	// Only the selected painter is enabled; others are disabled
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
			brush_decal->setEnabled(false);		// No brush preview needed
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

// Brush pattern texture is applied to both painters and preview decal.
void PaintSample::set_brush_texture(const Unigine::TexturePtr &in_brush_texture)
{
	brush_texture = in_brush_texture;

	// Both brush-based painters receive the texture
	albedo_painter->setBrushTexture(in_brush_texture);
	height_painter->setBrushTexture(in_brush_texture);

	// Brush preview decal is updated
	brush_decal_material->setTexture("albedo", brush_texture);
}

// Brush mask texture is applied to both painters and preview decal.
void PaintSample::set_brush_mask(const Unigine::TexturePtr &in_brush_mask)
{
	brush_mask = in_brush_mask;

	// Both brush-based painters receive the mask
	albedo_painter->setBrushMask(in_brush_mask);
	height_painter->setBrushMask(in_brush_mask);

	// Brush preview decal is updated
	brush_decal_material->setTexture("mask", brush_mask);
}

// Gradient texture is applied to height-to-albedo converter.
void PaintSample::set_gradient(const Unigine::TexturePtr &in_gradient)
{
	gradient = in_gradient;

	height_to_albedo->setGradient(gradient);
}

// Brush size is applied to painters and preview decal with optional GUI sync.
void PaintSample::set_brush_size(float in_brush_size, bool update_ui)
{
	// Minimum size is enforced
	brush_size = max(in_brush_size, 1.0f);

	// Both brush-based painters receive the size
	albedo_painter->setBrushSize(brush_size);
	height_painter->setBrushSize(brush_size);

	// Brush preview decal is resized
	brush_decal->setWidth(brush_size);
	brush_decal->setHeight(brush_size);

	// GUI edit lines are synchronized if requested
	if (update_ui)
	{
		// Callback is temporarily disconnected to avoid recursion
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

// Brush spacing is applied to both painters with optional GUI sync.
void PaintSample::set_brush_spacing(float in_brush_spacing, bool update_ui)
{
	// Minimum spacing is enforced
	brush_spacing = max(in_brush_spacing, 0.0f);

	// Both brush-based painters receive the spacing
	albedo_painter->setBrushSpacing(brush_spacing);
	height_painter->setBrushSpacing(brush_spacing);

	// GUI edit lines are synchronized if requested
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

// Brush angle is applied to painters and preview decal with optional GUI sync.
void PaintSample::set_brush_angle(float in_brush_angle, bool update_ui)
{
	brush_angle = in_brush_angle;

	// Both brush-based painters receive the angle
	albedo_painter->setBrushAngle(brush_angle);
	height_painter->setBrushAngle(brush_angle);

	// Brush preview decal rotation is updated (negated for visual match)
	brush_decal->setRotation(quat(0.0f, 0.0f, -brush_angle), true);

	// GUI edit lines are synchronized if requested
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

// Brush height value is applied to height painter with optional GUI sync.
void PaintSample::set_brush_height(float in_brush_height, bool update_ui)
{
	brush_height = in_brush_height;

	// Only height painter uses this parameter
	height_painter->setBrushHeight(in_brush_height);

	// GUI edit line is synchronized if requested
	if (update_ui)
	{
		height_brush_height_edit_line_callback.disconnect();
		height_brush_height_edit_line->setText(String::ftoa(brush_height));
		height_brush_height_edit_line->getEventChanged().connect(
			height_brush_height_edit_line_callback, this, &PaintSample::brush_height_edit_line_callback);
	}
}

// Brush height scale is applied to height painter with optional GUI sync.
void PaintSample::set_brush_height_scale(float in_brush_height_scale, bool update_ui)
{
	brush_height_scale = in_brush_height_scale;

	// Only height painter uses this parameter
	height_painter->setBrushHeightScale(in_brush_height_scale);

	// GUI edit line is synchronized if requested
	if (update_ui)
	{
		height_brush_height_scale_edit_line_callback.disconnect();
		height_brush_height_scale_edit_line->setText(String::ftoa(brush_height_scale));
		height_brush_height_scale_edit_line->getEventChanged().connect(
			height_brush_height_scale_edit_line_callback, this, &PaintSample::brush_height_scale_edit_line_callback);
	}
}

// Brush opacity is applied to both painters with optional GUI sync.
void PaintSample::set_brush_opacity(float in_brush_opacity, bool update_ui)
{
	// Opacity is clamped to valid range
	brush_opacity = clamp(in_brush_opacity, 0.0f, 1.0f);

	// Both brush-based painters receive the opacity
	albedo_painter->setBrushOpacity(brush_opacity);
	height_painter->setBrushOpacity(brush_opacity);

	// GUI edit lines are synchronized if requested
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

// Brush color is applied to albedo painter and preview decal with optional GUI sync.
void PaintSample::set_brush_color(const Unigine::Math::vec3 &in_brush_color, bool update_ui)
{
	// Color channels are clamped to valid range
	brush_color = clamp(in_brush_color, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	// Only albedo painter uses brush color
	albedo_painter->setBrushColor(brush_color);

	// Brush preview decal color is updated
	brush_decal_material->setParameterFloat4("albedo", {brush_color, 1.0f});

	// GUI edit lines are synchronized if requested (displayed as 0-255)
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

// Height blend mode is applied to height painter with optional GUI sync.
void PaintSample::set_height_blend_mode(BlendMode in_blend_mode, bool update_ui)
{
	blend_mode = in_blend_mode;

	// Only height painter uses blend mode
	height_painter->setBlendMode(in_blend_mode);

	// GUI combo box is synchronized if requested
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

// Minimum height for gradient mapping is applied with optional GUI sync.
void PaintSample::set_min_height(float in_min_height, bool update_ui)
{
	min_height = in_min_height;

	// Only height-to-albedo uses this parameter
	height_to_albedo->setMinHeight(min_height);

	// GUI edit line is synchronized if requested
	if (update_ui)
	{
		min_height_edit_line_callback_.disconnect();
		min_height_edit_line->setText(String::ftoa(min_height));
		min_height_edit_line->getEventChanged().connect(
			min_height_edit_line_callback_, this, &PaintSample::min_height_edit_line_callback);
	}
}

// Maximum height for gradient mapping is applied with optional GUI sync.
void PaintSample::set_max_height(float in_max_height, bool update_ui)
{
	max_height = in_max_height;

	// Only height-to-albedo uses this parameter
	height_to_albedo->setMaxHeight(max_height);

	// GUI edit line is synchronized if requested
	if (update_ui)
	{
		max_height_edit_line_callback_.disconnect();
		max_height_edit_line->setText(String::ftoa(max_height));
		max_height_edit_line->getEventChanged().connect(
			max_height_edit_line_callback_, this, &PaintSample::max_height_edit_line_callback);
	}
}

// Tabbed GUI is created with parameter controls for all painting modes.
void PaintSample::init_gui()
{
	// Sample window is created from reusable component
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	window = sample_description_window.getWindow();

	// Tab box is created for mode selection
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

// Sample window is released.
void PaintSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

// =====================================================================
// GUI CALLBACKS - Handle user interaction with controls
// =====================================================================

// Active painting mode is switched based on selected tab.
void PaintSample::tab_box_callback()
{
	switch (tab_box->getCurrentTab())
	{
		case 0: set_mode(Mode::AlbedoPainter); break;
		case 1: set_mode(Mode::HeightPainter); break;
		case 2: set_mode(Mode::HeightToAlbedo); break;
	}
}

// Brush texture is changed to selected index.
void PaintSample::brush_texture_button_callback(int index)
{
	set_brush_texture(brush_textures[index]);
}

// Brush mask is changed to selected index.
void PaintSample::brush_mask_button_callback(int index)
{
	set_brush_mask(brush_masks[index]);
}

// Gradient is changed and height-to-albedo conversion is triggered.
void PaintSample::gradient_button_callback(int index)
{
	set_gradient(gradients[index]);
	height_to_albedo->run();
}

// Brush size is updated from edit line text.
void PaintSample::brush_size_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_size(value, true);
}

// Brush spacing is updated from edit line text.
void PaintSample::brush_spacing_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_spacing(value, false);
}

// Brush angle is updated from edit line text.
void PaintSample::brush_angle_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_angle(value, false);
}

// Brush height is updated from edit line text.
void PaintSample::brush_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_height(value, false);
}

// Brush height scale is updated from edit line text.
void PaintSample::brush_height_scale_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_height_scale(value, false);
}

// Brush opacity is updated from edit line text.
void PaintSample::brush_opacity_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_opacity(value, false);
}

// Red color channel is updated from edit line text (0-255 to 0-1).
void PaintSample::brush_color_r_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({value, brush_color.y, brush_color.z}, false);
}

// Green color channel is updated from edit line text (0-255 to 0-1).
void PaintSample::brush_color_g_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({brush_color.x, value, brush_color.z}, false);
}

// Blue color channel is updated from edit line text (0-255 to 0-1).
void PaintSample::brush_color_b_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({brush_color.x, brush_color.y, value}, false);
}

// Minimum gradient height is updated from edit line text.
void PaintSample::min_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_min_height(value, false);
}

// Maximum gradient height is updated from edit line text.
void PaintSample::max_height_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText());
	set_max_height(value, false);
}

// Height blend mode is updated from combo box selection.
void PaintSample::blend_mode_combo_box_callback(const Unigine::WidgetPtr &widget)
{
	switch (static_ptr_cast<WidgetComboBox>(widget)->getCurrentItem())
	{
		case 0: set_height_blend_mode(BlendMode::Alpha); break;
		case 1: set_height_blend_mode(BlendMode::Additive); break;
	}
}

// Height-to-albedo conversion is triggered manually.
void PaintSample::height_to_albedo_run_button_callback()
{
	height_to_albedo->run();
}
