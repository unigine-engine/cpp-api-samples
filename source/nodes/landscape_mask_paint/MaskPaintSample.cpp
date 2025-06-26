#include "MaskPaintSample.h"

#include <UnigineRender.h>

REGISTER_COMPONENT(MaskPaintSample);

using namespace Unigine;
using namespace Math;

void MaskPaintSample::init()
{
	saved_mouse_handle = ControlsApp::getMouseHandle();
	ControlsApp::setMouseHandle(Input::MOUSE_HANDLE_USER);

	mask_painter = ComponentSystem::get()->getComponent<LandscapeMaskPainter>(
		mask_painter_param.get());

	player = checked_ptr_cast<PlayerSpectator>(player_param.get());
	landscape_fetch = LandscapeFetch::create();

	auto guid = FileSystem::getGUID(
		FileSystem::resolvePartialVirtualPath("decal_brush_base.basemat"));

	if (!guid.isValid())
	{
		Log::error("MaskPaintSample::init(): can not find "
				   "\"decal_brush_base.basemat\" material");
		return;
	}

	brush_decal_material = Materials::findMaterialByFileGUID(guid)->inherit();
	brush_decal = DecalOrtho::create(); // 100.f, 1.f, 1.f, "decal_base"
	brush_decal->setRadius(100.f);
	brush_decal->setWidth(1.f);
	brush_decal->setMaterial(brush_decal_material);

	// show only mask data
	Render::setShowLandscapeMask(1);

	init_resources();
	init_gui();

	landscape_layer_map = checked_ptr_cast<LandscapeLayerMap>(landscape_layer_map_param.get());
	if (!landscape_layer_map)
	{
		Log::error("MaskPaintSample::init(): can not find landscape layer map");
		return;
	}

	mask_painter->enable();

	mask_painter->setTarget(landscape_layer_map);
	set_brush_texture(brush_textures.begin().get());
	set_brush_mask(brush_textures.begin().get());
	set_brush_size(brush_size);
	set_brush_spacing(brush_spacing);
	set_brush_angle(brush_angle);
	set_brush_opacity(brush_opacity);
	set_brush_color(brush_color);
}

void MaskPaintSample::init_resources()
{
	for (int i = 0; i < brush_textures_param.size(); ++i)
	{
		auto texture = Texture::create();
		texture->load(brush_textures_param.get(i));
		brush_textures.push_back(texture);

		auto image = Image::create();
		image->load(brush_textures_param.get(i));
		image->resize(32, 32);
		brush_textures_icons.push_back(image);
	}

	for (int i = 0; i < brush_masks_param.size(); ++i)
	{
		auto texture = Texture::create();
		texture->load(brush_masks_param.get(i));
		brush_masks.push_back(texture);

		auto image = Image::create();
		image->load(brush_masks_param.get(i));
		image->resize(32, 32);
		brush_masks_icons.push_back(image);
	}

	for (int i = 0; i < gradients_param.size(); ++i)
	{
		auto texture = Texture::create();
		texture->load(gradients_param.get(i));
		gradients.push_back(texture);

		auto image = Image::create();
		image->load(gradients_param.get(i));
		image->resize(32, 32);
		gradients_icons.push_back(image);
	}
}

void MaskPaintSample::update()
{
	if (window->getMouseX() >= 0 && window->getMouseX() < window->getWidth()
		&& window->getMouseY() >= 0 && window->getMouseY() < window->getHeight())
	{
		return;
	}

	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT))
	{
		ivec2 delta = Input::getMouseDeltaPosition();
		player->setPhiAngle(player->getPhiAngle() + static_cast<float>(delta.x));
		player->setThetaAngle(player->getThetaAngle() + static_cast<float>(delta.y));
	}

	if (Input::isKeyPressed(Input::KEY_LEFT_SHIFT) || Input::isKeyPressed(Input::KEY_RIGHT_SHIFT))
	{
		set_brush_angle(brush_angle + static_cast<float>(Input::getMouseWheel()) * 5);
	}
	else if (Input::isKeyPressed(Input::KEY_LEFT_CTRL)
		|| Input::isKeyPressed(Input::KEY_RIGHT_CTRL))
	{
		set_brush_size(brush_size + static_cast<float>(Input::getMouseWheel()) * 0.5f);
	}

	const auto mouse_coord = Input::getMousePosition();
	bool fetched = landscape_fetch->intersectionForce(player->getPosition(),
		player->getPosition()
			+ Vec3(player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y)) * 10000.0);

	if (fetched)
	{
		brush_decal->setPosition(landscape_fetch->getPosition() + Vec3(0.0, 0.0, 50.0));

		if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
		{
			mask_painter->paintAt(landscape_fetch->getPosition());
		}
	}
}

void MaskPaintSample::shutdown()
{
	mask_painter->disable();
	ControlsApp::setMouseHandle(saved_mouse_handle);

	window.deleteLater();

	Render::setShowLandscapeMask(0);
}

void MaskPaintSample::set_brush_texture(const TexturePtr &in_brush_texture)
{
	brush_decal_material->setTexture("albedo", in_brush_texture);
	mask_painter->setBrushTexture(in_brush_texture);
}

void MaskPaintSample::set_brush_mask(const TexturePtr &in_brush_mask)
{
	brush_decal_material->setTexture("mask", in_brush_mask);
	mask_painter->setBrushMask(in_brush_mask);
}

void MaskPaintSample::set_brush_size(float in_brush_size, bool update_ui)
{
	brush_size = max(in_brush_size, 1.0f);

	mask_painter->setBrushSize(brush_size);
	brush_decal->setWidth(brush_size);
	brush_decal->setHeight(brush_size);

	if (update_ui)
	{
		brush_size_connection.disconnect();
		brush_size_edit_line->setText(String::ftoa(brush_size));
		brush_size_edit_line->getEventChanged().connect(brush_size_connection, this,
			&MaskPaintSample::brush_size_edit_line_callback);
	}
}

void MaskPaintSample::set_brush_spacing(float in_brush_spacing, bool update_ui)
{
	brush_spacing = max(in_brush_spacing, 0.f);

	mask_painter->setBrushSpacing(brush_spacing);

	if (update_ui)
	{
		brush_spacing_connection.disconnect();
		brush_spacing_edit_line->setText(String::ftoa(brush_spacing));
		brush_spacing_edit_line->getEventChanged().connect(brush_spacing_connection, this,
			&MaskPaintSample::brush_spacing_edit_line_callback);
	}
}

void MaskPaintSample::set_brush_angle(float in_brush_angle, bool update_ui)
{
	brush_angle = in_brush_angle;

	mask_painter->setBrushAngle(in_brush_angle);
	brush_decal->setRotation(quat(0.f, 0.f, -brush_angle), true);
	if (update_ui)
	{
		brush_angle_connection.disconnect();
		brush_angle_edit_line->setText(String::ftoa(brush_angle));
		brush_angle_edit_line->getEventChanged().connect(brush_angle_connection, this,
			&MaskPaintSample::brush_angle_edit_line_callback);
	}
}

void MaskPaintSample::set_brush_opacity(float in_brush_opacity, bool update_ui)
{
	brush_opacity = clamp(in_brush_opacity, 0.0f, 1.f);

	mask_painter->setBrushOpacity(brush_opacity);

	if (update_ui)
	{
		brush_opacity_connection.disconnect();
		brush_opacity_edit_line->setText(String::ftoa(brush_opacity));
		brush_opacity_edit_line->getEventChanged().connect(brush_opacity_connection, this,
			&MaskPaintSample::brush_opacity_edit_line_callback);
	}
}

void MaskPaintSample::set_brush_color(const vec4 &in_brush_color, bool update_ui)
{
	brush_color = clamp(in_brush_color, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
	mask_painter->setBrushColor(in_brush_color);

	// because we are drawing only in red channel
	const vec4 color_for_decal = vec4{brush_color.x, brush_color.x, brush_color.x, 1.f};

	brush_decal_material->setParameterFloat4("albedo", color_for_decal);
	if (update_ui)
	{
		brush_mask_value_connection.disconnect();
		brush_mask_value_edit_line->setText(String::ftoa(brush_color.x * 255));
		brush_mask_value_edit_line->getEventChanged().connect(brush_mask_value_connection, this,
			&MaskPaintSample::brush_mask_value_edit_line_callback);
	}
}

void MaskPaintSample::init_gui()
{
	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();

	window = sample_description_window.getWindow();

	auto hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	auto label = WidgetLabel::create("Texture");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	auto gridbox = WidgetGridBox::create(6);
	parameters->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_textures.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_textures_icons[i]);
		button->getEventClicked().connect(this, &MaskPaintSample::brush_texture_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Mask");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	gridbox = WidgetGridBox::create(6);
	parameters->addChild(gridbox, Gui::ALIGN_LEFT);
	for (int i = 0; i < brush_masks.size(); ++i)
	{
		auto button = WidgetButton::create();
		button->setImage(brush_masks_icons[i]);
		button->getEventClicked().connect(this, &MaskPaintSample::brush_mask_button_callback, i);
		gridbox->addChild(button, Gui::ALIGN_LEFT);
	}

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Size");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	brush_size_edit_line = WidgetEditLine::create();
	brush_size_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(brush_size_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Spacing");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	brush_spacing_edit_line = WidgetEditLine::create();
	brush_spacing_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(brush_spacing_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Angle");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	brush_angle_edit_line = WidgetEditLine::create();
	brush_angle_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(brush_angle_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);
	label = WidgetLabel::create("Opacity");
	label->setWidth(100);
	hbox->addChild(label, Gui::ALIGN_LEFT);
	brush_opacity_edit_line = WidgetEditLine::create();
	brush_opacity_edit_line->setValidator(Gui::VALIDATOR_FLOAT);
	hbox->addChild(brush_opacity_edit_line, Gui::ALIGN_LEFT);

	hbox = WidgetHBox::create(5);
	parameters->addChild(hbox, Gui::ALIGN_LEFT);

	label = WidgetLabel::create("Mask Value");
	hbox->addChild(label, Gui::ALIGN_LEFT);
	brush_mask_value_edit_line = WidgetEditLine::create();
	brush_mask_value_edit_line->setValidator(Gui::VALIDATOR_UINT);
	hbox->addChild(brush_mask_value_edit_line, Gui::ALIGN_LEFT);
}

void MaskPaintSample::shutdown_gui()
{
	sample_description_window.shutdown();
}

void MaskPaintSample::brush_texture_button_callback(int index)
{
	set_brush_texture(brush_textures[index]);
}

void MaskPaintSample::brush_mask_button_callback(int index)
{
	set_brush_mask(brush_masks[index]);
}

void MaskPaintSample::brush_size_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = max(String::atof(checked_ptr_cast<WidgetEditLine>(widget)->getText()), 0.1f);
	set_brush_size(value, true);
}

void MaskPaintSample::brush_spacing_edit_line_callback(const WidgetPtr &widget)
{
	float value = String::atof(checked_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_spacing(value, false);
}

void MaskPaintSample::brush_angle_edit_line_callback(const WidgetPtr &widget)
{
	float value = String::atof(checked_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_angle(value, false);
}

void MaskPaintSample::brush_opacity_edit_line_callback(const WidgetPtr &widget)
{
	float value = String::atof(checked_ptr_cast<WidgetEditLine>(widget)->getText());
	set_brush_opacity(value, false);
}

void MaskPaintSample::brush_mask_value_edit_line_callback(const Unigine::WidgetPtr &widget)
{
	float value = String::atof(static_ptr_cast<WidgetEditLine>(widget)->getText()) / 255.0f;
	set_brush_color({value, brush_color.y, brush_color.z, brush_color.w}, true);
}
