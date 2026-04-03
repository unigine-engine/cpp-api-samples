// Paints height values onto terrain using configurable brush strokes.
// Supports alpha and additive blend modes for terrain sculpting.
// Brush strokes are applied via async texture draw with GPU shader.

#pragma once
#include <UnigineComponentSystem.h>

// Height painting blend mode selection.
enum class BlendMode
{
	Alpha = 0,		// Replaces height values with brush height
	Additive = 1	// Adds brush height to existing terrain height
};

// Applies brush-based height painting to landscape layer maps.
class LandscapeHeightPainter : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapeHeightPainter, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	void enable();		// Connects async texture draw callback
	void disable();		// Disconnects async texture draw callback

	void setTarget(const Unigine::LandscapeLayerMapPtr &in_landscape_layer_map) { landscape_layer_map = in_landscape_layer_map; }	// Sets target landscape

	void setBlendMode(BlendMode in_blend_mode) { blend_mode = in_blend_mode; }							// Sets height blending mode
	void setBrushTexture(const Unigine::TexturePtr &in_brush_texture) { brush_texture = in_brush_texture; }		// Sets brush pattern texture
	void setBrushMask(const Unigine::TexturePtr &in_brush_mask) { brush_mask = in_brush_mask; }					// Sets brush opacity mask
	void setBrushSize(float in_brush_size) { brush_size = in_brush_size; }										// Sets brush diameter in world units
	void setBrushSpacing(float in_brush_spacing) { brush_spacing = in_brush_spacing; }							// Sets stroke spacing as fraction of size
	void setBrushAngle(float in_brush_angle) { brush_angle = in_brush_angle; }									// Sets brush rotation angle in degrees
	void setBrushOpacity(float in_brush_opacity) { brush_opacity = in_brush_opacity; }							// Sets brush opacity multiplier
	void setBrushHeight(float in_brush_height) { brush_height = in_brush_height; }								// Sets target height value
	void setBrushHeightScale(float in_brush_height_scale) { brush_height_scale = in_brush_height_scale; }		// Sets height texture scale factor

	void paintAt(const Unigine::Math::Vec3 &world_position);	// Applies brush stroke at world position

private:
	void init();
	// Callback invoked when async texture draw buffer is ready
	void texture_draw_callback(const Unigine::UGUID &guid, int id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask);

	Unigine::MaterialPtr brush_material;			// Shader material for brush rendering
	Unigine::Math::Vec3 last_paint_position;		// Previous stroke position for spacing

	Unigine::LandscapeLayerMapPtr landscape_layer_map;	// Target landscape for painting

	// Brush parameters
	BlendMode blend_mode{ BlendMode::Alpha };			// Alpha or additive height blending
	Unigine::TexturePtr brush_texture;					// Pattern texture for brush shape
	Unigine::TexturePtr brush_mask;						// Opacity mask for brush falloff
	float brush_size{ 1.0f };							// Brush diameter in world units
	float brush_spacing{ 0.2f };						// Stroke spacing as fraction of size
	float brush_angle{ 0.0f };							// Brush rotation in degrees
	float brush_opacity{ 1.0f };						// Overall opacity multiplier
	float brush_height{ 0.0f };							// Target terrain height value
	float brush_height_scale{ 1.0f };					// Multiplier for brush texture height

	// Parameters captured per async draw operation.
	struct DrawData
	{
		BlendMode blend_mode;					// Blend mode for this operation
		Unigine::TexturePtr brush_texture;		// Brush pattern for this operation
		Unigine::TexturePtr brush_mask;			// Brush mask for this operation
		float brush_angle;						// Rotation angle for this operation
		float brush_opacity;					// Opacity for this operation
		float brush_height;						// Height value for this operation
		float brush_height_scale;				// Height scale for this operation

		DrawData(
			BlendMode blend_mode,
			const Unigine::TexturePtr &brush_texture,
			const Unigine::TexturePtr &brush_mask,
			float brush_angle,
			float brush_opacity,
			float brush_height,
			float brush_height_scale)
			: blend_mode{ blend_mode }
			, brush_texture{ brush_texture }
			, brush_mask{ brush_mask }
			, brush_angle{ brush_angle }
			, brush_opacity{ brush_opacity }
			, brush_height{ brush_height }
			, brush_height_scale{ brush_height_scale }
		{ }
	};
	Unigine::HashMap<int, DrawData> per_operation_draw_data;	// Pending operations by ID

	Unigine::EventConnection callback_handle;	// Handle for texture draw event
};
