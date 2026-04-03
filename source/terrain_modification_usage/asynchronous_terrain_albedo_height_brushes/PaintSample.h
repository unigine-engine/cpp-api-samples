// Interactive GUI for landscape painting with albedo, height, and gradient modes.
// Coordinates multiple painter components and provides brush preview visualization.
// Supports real-time parameter editing with keyboard and mouse shortcuts.

#pragma once

#include "LandscapeAlbedoPainter.h"
#include "LandscapeHeightPainter.h"
#include "LandscapeHeightToAlbedo.h"
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineWidgets.h>
#include <UnigineDecals.h>

// Main controller for terrain painting sample with tabbed GUI interface.
class PaintSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PaintSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);


	// References to painter component nodes
	PROP_PARAM(Node, albedo_painter_param, "Albedo Painter");		// Node with LandscapeAlbedoPainter
	PROP_PARAM(Node, height_painter_param, "Height Painter");		// Node with LandscapeHeightPainter
	PROP_PARAM(Node, height_to_albedo_param, "Height To Albedo");	// Node with LandscapeHeightToAlbedo

	PROP_PARAM(Node, landscape_layer_map_param, "Landscape Layer Map");	// Target terrain layer
	PROP_PARAM(Node, player_param, "Player");							// Camera for ray casting

	// Resource arrays for brush selection
	PROP_ARRAY(File, brush_textures_param, "Brush Textures");	// Available brush patterns
	PROP_ARRAY(File, brush_masks_param, "Brush Masks");			// Available brush masks
	PROP_ARRAY(File, gradients_param, "Gradients");				// Available height-to-albedo gradients

private:
	void init();
	void init_resources();		// Loads brush textures, masks, and gradients
	void update();
	void shutdown();

	Unigine::LandscapeLayerMapPtr landscape_layer_map;		// Target terrain for painting
	Unigine::PlayerSpectatorPtr player;						// Camera for mouse ray casting
	Unigine::LandscapeFetchPtr landscape_fetch;				// Terrain intersection query
	Unigine::DecalOrthoPtr brush_decal;						// Visual brush preview on terrain
	Unigine::MaterialPtr brush_decal_material;				// Material for brush preview
	Unigine::Input::MOUSE_HANDLE saved_mouse_handle{ Unigine::Input::MOUSE_HANDLE_GRAB };	// Original mouse mode

	// Painting mode selection.
	enum class Mode
	{
		AlbedoPainter,		// Brush-based albedo painting
		HeightPainter,		// Brush-based height sculpting
		HeightToAlbedo		// Gradient-based colorization
	};
	Mode mode{ Mode::AlbedoPainter };	// Current active painting mode
	void set_mode(Mode mode);			// Switches between painting modes

	// Painter component references
	LandscapeAlbedoPainter *albedo_painter{ nullptr };		// Albedo painting component
	LandscapeHeightPainter *height_painter{ nullptr };		// Height painting component
	LandscapeHeightToAlbedo *height_to_albedo{ nullptr };	// Height-to-albedo component

	// Loaded texture resources
	Unigine::Vector<Unigine::TexturePtr> brush_textures;	// Full-resolution brush patterns
	Unigine::Vector<Unigine::TexturePtr> brush_masks;		// Full-resolution brush masks
	Unigine::Vector<Unigine::TexturePtr> gradients;			// Full-resolution gradients

	// GUI icon images (downscaled from textures)
	Unigine::Vector<Unigine::ImagePtr> brush_textures_icons;	// Brush texture thumbnails
	Unigine::Vector<Unigine::ImagePtr> brush_masks_icons;		// Brush mask thumbnails
	Unigine::Vector<Unigine::ImagePtr> gradiends_icons;			// Gradient thumbnails

	// Parameter setters with optional GUI synchronization
	void set_brush_texture(const Unigine::TexturePtr &brush_texture);		// Sets current brush pattern
	void set_brush_mask(const Unigine::TexturePtr &brush_mask);				// Sets current brush mask
	void set_gradient(const Unigine::TexturePtr &gradient);					// Sets height-to-albedo gradient
	void set_brush_size(float brush_size, bool update_ui = true);			// Sets brush diameter
	void set_brush_spacing(float brush_spacing, bool update_ui = true);		// Sets stroke spacing
	void set_brush_angle(float brush_angle, bool update_ui = true);			// Sets brush rotation
	void set_brush_height(float brush_height, bool update_ui = true);		// Sets target height value
	void set_brush_height_scale(float brush_height_scale, bool update_ui = true);	// Sets height scale
	void set_brush_opacity(float brush_opacity, bool update_ui = true);		// Sets brush opacity
	void set_brush_color(const Unigine::Math::vec3 &brush_color, bool update_ui = true);	// Sets brush color
	void set_height_blend_mode(BlendMode blend_mode, bool update_ui = true);	// Sets alpha/additive mode
	void set_min_height(float min_height, bool update_ui = true);			// Sets gradient min height
	void set_max_height(float max_height, bool update_ui = true);			// Sets gradient max height

	// Current brush parameter values
	Unigine::TexturePtr brush_texture;					// Active brush pattern texture
	Unigine::TexturePtr brush_mask;						// Active brush mask texture
	Unigine::TexturePtr gradient;						// Active gradient texture
	float brush_size{ 100.0f };							// Brush diameter in world units
	float brush_spacing{ 0.2f };						// Stroke spacing as size fraction
	float brush_angle{ 0.0f };							// Brush rotation in degrees
	float brush_height{ 0.0f };							// Target terrain height
	float brush_height_scale{ 0.5f };					// Height texture multiplier
	float brush_opacity{ 1.0f };						// Overall opacity multiplier
	Unigine::Math::vec3 brush_color{ 1.0f, 1.0f, 1.0f };	// RGB brush color
	BlendMode blend_mode{ BlendMode::Alpha };			// Height blending mode
	float min_height{ 0.0f };							// Gradient mapping minimum
	float max_height{ 1.0f };							// Gradient mapping maximum


	// GUI initialization and cleanup
	void init_gui();		// Creates tabbed interface with parameter controls
	void shutdown_gui();	// Releases GUI resources

	Unigine::WidgetWindowPtr window;		// Main sample window
	Unigine::WidgetTabBoxPtr tab_box;		// Mode selection tabs

	// Albedo painter tab widgets
	Unigine::WidgetEditLinePtr albedo_brush_size_edit_line;			// Brush size input
	Unigine::WidgetEditLinePtr albedo_brush_spacing_edit_line;		// Brush spacing input
	Unigine::WidgetEditLinePtr albedo_brush_angle_edit_line;		// Brush angle input
	Unigine::WidgetEditLinePtr albedo_brush_opacity_edit_line;		// Brush opacity input
	Unigine::WidgetEditLinePtr albedo_brush_color_r_edit_line;		// Color red channel
	Unigine::WidgetEditLinePtr albedo_brush_color_g_edit_line;		// Color green channel
	Unigine::WidgetEditLinePtr albedo_brush_color_b_edit_line;		// Color blue channel

	// Height painter tab widgets
	Unigine::WidgetEditLinePtr height_brush_size_edit_line;			// Brush size input
	Unigine::WidgetEditLinePtr height_brush_spacing_edit_line;		// Brush spacing input
	Unigine::WidgetEditLinePtr height_brush_angle_edit_line;		// Brush angle input
	Unigine::WidgetEditLinePtr height_brush_opacity_edit_line;		// Brush opacity input
	Unigine::WidgetEditLinePtr height_brush_height_edit_line;		// Target height input
	Unigine::WidgetEditLinePtr height_brush_height_scale_edit_line;	// Height scale input
	Unigine::WidgetComboBoxPtr height_blend_mode_combo_box;			// Alpha/additive selector

	// Height-to-albedo tab widgets
	Unigine::WidgetEditLinePtr min_height_edit_line;	// Gradient minimum height
	Unigine::WidgetEditLinePtr max_height_edit_line;	// Gradient maximum height

	SampleDescriptionWindow sample_description_window;	// Reusable window component

	// GUI event callbacks
	void tab_box_callback();											// Handles tab selection
	void brush_texture_button_callback(int index);						// Handles texture selection
	void brush_mask_button_callback(int index);							// Handles mask selection
	void gradient_button_callback(int index);							// Handles gradient selection
	void brush_size_edit_line_callback(const Unigine::WidgetPtr &widget);		// Handles size change
	void brush_spacing_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles spacing change
	void brush_angle_edit_line_callback(const Unigine::WidgetPtr &widget);		// Handles angle change
	void brush_height_edit_line_callback(const Unigine::WidgetPtr &widget);		// Handles height change
	void brush_height_scale_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles scale change
	void brush_opacity_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles opacity change
	void brush_color_r_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles red channel
	void brush_color_g_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles green channel
	void brush_color_b_edit_line_callback(const Unigine::WidgetPtr &widget);	// Handles blue channel
	void blend_mode_combo_box_callback(const Unigine::WidgetPtr &widget);		// Handles blend mode
	void min_height_edit_line_callback(const Unigine::WidgetPtr &widget);		// Handles min height
	void max_height_edit_line_callback(const Unigine::WidgetPtr &widget);		// Handles max height
	void height_to_albedo_run_button_callback();								// Triggers conversion

	// Event connection handles for callback management
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
