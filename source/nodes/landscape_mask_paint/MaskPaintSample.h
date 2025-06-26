#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "LandscapeMaskPainter.h"

#include <UnigineComponentSystem.h>
#include <UnigineDecals.h>
#include <UnigineObjects.h>
#include <UniginePlayers.h>

class MaskPaintSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(MaskPaintSample, Unigine::ComponentBase)

	PROP_PARAM(Node, mask_painter_param, "Mask Painter");
	PROP_PARAM(Node, landscape_layer_map_param, "Landscape Layer Map");
	PROP_PARAM(Node, player_param, "Player");

	PROP_ARRAY(File, brush_textures_param, "Brush Textures");
	PROP_ARRAY(File, brush_masks_param, "Brush Masks");
	PROP_ARRAY(File, gradients_param, "Gradients");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void set_brush_texture(const Unigine::TexturePtr &in_brush_texture);
	void set_brush_mask(const Unigine::TexturePtr &in_brush_mask);
	void set_brush_size(float in_brush_size, bool update_ui = true);
	void set_brush_spacing(float in_brush_spacing, bool update_ui = true);
	void set_brush_angle(float in_brush_angle, bool update_ui = true);
	void set_brush_opacity(float in_brush_opacity, bool update_ui = true);
	void set_brush_color(const Unigine::Math::vec4 &in_brush_color, bool update_ui = true);

	// Gui
	void init_gui();
	void shutdown_gui();

private:
	void init();
	void init_resources();
	void update();
	void shutdown();

	LandscapeMaskPainter *mask_painter = nullptr;

	Unigine::LandscapeLayerMapPtr landscape_layer_map = nullptr;
	Unigine::PlayerSpectatorPtr player;
	Unigine::LandscapeFetchPtr landscape_fetch;
	Unigine::DecalOrthoPtr brush_decal;
	Unigine::MaterialPtr brush_decal_material;
	Unigine::Input::MOUSE_HANDLE saved_mouse_handle{Unigine::Input::MOUSE_HANDLE_GRAB};

	Unigine::Vector<Unigine::TexturePtr> brush_textures;
	Unigine::Vector<Unigine::TexturePtr> brush_masks;
	Unigine::Vector<Unigine::TexturePtr> gradients;

	Unigine::Vector<Unigine::ImagePtr> brush_textures_icons;
	Unigine::Vector<Unigine::ImagePtr> brush_masks_icons;
	Unigine::Vector<Unigine::ImagePtr> gradients_icons;

	Unigine::TexturePtr brush_texture;
	Unigine::TexturePtr brush_mask;
	Unigine::TexturePtr gradient;
	float brush_size{100.f};
	float brush_spacing{0.2f};
	float brush_angle{0.f};
	float brush_opacity{1.f};
	Unigine::Math::vec4 brush_color{0.5f, 0.f, 0.f, 1.f};

	// Gui

	Unigine::WidgetWindowPtr window;

	Unigine::WidgetEditLinePtr brush_size_edit_line;
	Unigine::WidgetEditLinePtr brush_spacing_edit_line;
	Unigine::WidgetEditLinePtr brush_angle_edit_line;
	Unigine::WidgetEditLinePtr brush_opacity_edit_line;
	Unigine::WidgetEditLinePtr brush_mask_value_edit_line;

	SampleDescriptionWindow sample_description_window;

	void brush_texture_button_callback(int index);
	void brush_mask_button_callback(int index);
	void brush_size_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_spacing_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_angle_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_opacity_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_mask_value(const Unigine::WidgetPtr &widget);
	void brush_mask_value_edit_line_callback(const Unigine::WidgetPtr &widget);

	Unigine::EventConnection brush_size_connection;
	Unigine::EventConnection brush_spacing_connection;
	Unigine::EventConnection brush_angle_connection;
	Unigine::EventConnection brush_opacity_connection;
	Unigine::EventConnection brush_mask_value_connection;
};
