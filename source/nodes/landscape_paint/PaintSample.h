#pragma once

#include "LandscapeAlbedoPainter.h"
#include "LandscapeHeightPainter.h"
#include "LandscapeHeightToAlbedo.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineWidgets.h>
#include <UnigineDecals.h>

class PaintSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PaintSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);


	PROP_PARAM(Node, albedo_painter_param, "Albedo Painter");
	PROP_PARAM(Node, height_painter_param, "Height Painter");
	PROP_PARAM(Node, height_to_albedo_param, "Height To Albedo");

	PROP_PARAM(Node, landscape_layer_map_param, "Landscape Layer Map");
	PROP_PARAM(Node, player_param, "Player");


	PROP_ARRAY(File, brush_textures_param, "Brush Textures");
	PROP_ARRAY(File, brush_masks_param, "Brush Masks");
	PROP_ARRAY(File, gradients_param, "Gradients");

private:
	void init();
	void init_resources();
	void update();
	void shutdown();

	Unigine::LandscapeLayerMapPtr landscape_layer_map;
	Unigine::PlayerSpectatorPtr player;
	Unigine::LandscapeFetchPtr landscape_fetch;
	Unigine::DecalOrthoPtr brush_decal;
	Unigine::MaterialPtr brush_decal_material;
	Unigine::Input::MOUSE_HANDLE saved_mouse_handle{ Unigine::Input::MOUSE_HANDLE_GRAB };

	enum class Mode
	{
		AlbedoPainter,
		HeightPainter,
		HeightToAlbedo
	};
	Mode mode{ Mode::AlbedoPainter };
	void set_mode(Mode mode);


	LandscapeAlbedoPainter *albedo_painter{ nullptr };
	LandscapeHeightPainter *height_painter{ nullptr };
	LandscapeHeightToAlbedo *height_to_albedo{ nullptr };


	Unigine::Vector<Unigine::TexturePtr> brush_textures;
	Unigine::Vector<Unigine::TexturePtr> brush_masks;
	Unigine::Vector<Unigine::TexturePtr> gradients;

	Unigine::Vector<Unigine::ImagePtr> brush_textures_icons;
	Unigine::Vector<Unigine::ImagePtr> brush_masks_icons;
	Unigine::Vector<Unigine::ImagePtr> gradiends_icons;

	// Parameters
	void set_brush_texture(const Unigine::TexturePtr &brush_texture);
	void set_brush_mask(const Unigine::TexturePtr &brush_mask);
	void set_gradient(const Unigine::TexturePtr &gradient);
	void set_brush_size(float brush_size, bool update_ui = true);
	void set_brush_spacing(float brush_spacing, bool update_ui = true);
	void set_brush_angle(float brush_angle, bool update_ui = true);
	void set_brush_height(float brush_height, bool update_ui = true);
	void set_brush_height_scale(float brush_height_scale, bool update_ui = true);
	void set_brush_opacity(float brush_opacity, bool update_ui = true);
	void set_brush_color(const Unigine::Math::vec3 &brush_color, bool update_ui = true);
	void set_height_blend_mode(BlendMode blend_mode, bool update_ui = true);
	void set_min_height(float min_height, bool update_ui = true);
	void set_max_height(float max_height, bool update_ui = true);

	Unigine::TexturePtr brush_texture;
	Unigine::TexturePtr brush_mask;
	Unigine::TexturePtr gradient;
	float brush_size{ 100.0f };
	float brush_spacing{ 0.2f };
	float brush_angle{ 0.0f };
	float brush_height{ 0.0f };
	float brush_height_scale{ 0.5f };
	float brush_opacity{ 1.0f };
	Unigine::Math::vec3 brush_color{ 1.0f, 1.0f, 1.0f };
	BlendMode blend_mode{ BlendMode::Alpha };
	float min_height{ 0.0f };
	float max_height{ 1.0f };


	// Gui stuff
	void init_gui();
	void shutdown_gui();

	Unigine::WidgetWindowPtr window;
	Unigine::WidgetTabBoxPtr tab_box;

	Unigine::WidgetEditLinePtr albedo_brush_size_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_spacing_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_angle_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_opacity_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_color_r_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_color_g_edit_line;
	Unigine::WidgetEditLinePtr albedo_brush_color_b_edit_line;

	Unigine::WidgetEditLinePtr height_brush_size_edit_line;
	Unigine::WidgetEditLinePtr height_brush_spacing_edit_line;
	Unigine::WidgetEditLinePtr height_brush_angle_edit_line;
	Unigine::WidgetEditLinePtr height_brush_opacity_edit_line;
	Unigine::WidgetEditLinePtr height_brush_height_edit_line;
	Unigine::WidgetEditLinePtr height_brush_height_scale_edit_line;
	Unigine::WidgetComboBoxPtr height_blend_mode_combo_box;

	Unigine::WidgetEditLinePtr min_height_edit_line;
	Unigine::WidgetEditLinePtr max_height_edit_line;

	SampleDescriptionWindow sample_description_window;

	void tab_box_callback();
	void brush_texture_button_callback(int index);
	void brush_mask_button_callback(int index);
	void gradient_button_callback(int index);
	void brush_size_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_spacing_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_angle_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_height_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_height_scale_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_opacity_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_color_r_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_color_g_edit_line_callback(const Unigine::WidgetPtr &widget);
	void brush_color_b_edit_line_callback(const Unigine::WidgetPtr &widget);
	void blend_mode_combo_box_callback(const Unigine::WidgetPtr &widget);
	void min_height_edit_line_callback(const Unigine::WidgetPtr &widget);
	void max_height_edit_line_callback(const Unigine::WidgetPtr &widget);
	void height_to_albedo_run_button_callback();

	Unigine::EventConnection albedo_brush_size_edit_line_callback;
	Unigine::EventConnection height_brush_size_edit_line_callback;
	Unigine::EventConnection albedo_brush_spacing_edit_line_callback;
	Unigine::EventConnection height_brush_spacing_edit_line_callback;
	Unigine::EventConnection albedo_brush_angle_edit_line_callback;
	Unigine::EventConnection height_brush_angle_edit_line_callback;
	Unigine::EventConnection height_brush_height_edit_line_callback;
	Unigine::EventConnection height_brush_height_scale_edit_line_callback;
	Unigine::EventConnection albedo_brush_opacity_edit_line_callback;
	Unigine::EventConnection height_brush_opacity_edit_line_callback;
	Unigine::EventConnection albedo_brush_color_r_edit_line_callback;
	Unigine::EventConnection albedo_brush_color_g_edit_line_callback;
	Unigine::EventConnection albedo_brush_color_b_edit_line_callback;
	Unigine::EventConnection height_blend_mode_combo_box_callback;
	Unigine::EventConnection min_height_edit_line_callback_;
	Unigine::EventConnection max_height_edit_line_callback_;

};
