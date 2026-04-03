// Implements terrain mask painting with configurable brush parameters.
// Brush strokes are applied to LandscapeLayerMap mask channels using async texture draw.
// Supports brush texture, opacity, rotation, spacing, and color customization.

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineTextures.h>

#include <utility>

// Paints mask data onto landscape terrain using customizable brush strokes.
class LandscapeMaskPainter : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapeMaskPainter, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	PROP_PARAM(Node, clutter_param);	// Mesh clutter to invalidate after painting

	void enable();		// Connects async texture draw callback
	void disable();		// Disconnects async texture draw callback

	// Sets the target landscape layer map for painting operations.
	void setTarget(const Unigine::LandscapeLayerMapPtr &in_landscape_layer_map)
	{
		landscape_layer_map = in_landscape_layer_map;
	}

	// Sets the brush texture pattern used for painting strokes.
	void setBrushTexture(const Unigine::TexturePtr &in_brush_texture)
	{
		brush_texture = in_brush_texture;
	}

	// Sets the brush mask texture for alpha masking.
	void setBrushMask(const Unigine::TexturePtr &in_brush_texture)
	{
		brush_mask = in_brush_texture;
	}

	void setBrushSize(float in_brush_size) { brush_size = in_brush_size; }				// Brush diameter in world units
	void setBrushSpacing(float in_brush_spacing) { brush_spacing = in_brush_spacing; }	// Spacing between strokes as ratio of brush size
	void setBrushAngle(float in_brush_angle) { brush_angle = in_brush_angle; }			// Brush rotation angle in degrees
	void setBrushOpacity(float in_brush_opacity) { brush_opacity = in_brush_opacity; }	// Brush opacity (0.0 to 1.0)
	void setBrushColor(const Unigine::Math::vec4 &in_brush_color) { brush_color = in_brush_color; }	// Brush color multiplier

	void paintAt(const Unigine::Math::Vec3 &world_position);	// Applies brush stroke at world position

private:
	void init();
	// Callback invoked when async texture draw is ready for shader execution
	void texture_draw_callback(const Unigine::UGUID &guid, int id,
		const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord,
		int data_mask);


private:
	Unigine::MaterialPtr brush_material = nullptr;		// Material used for brush rendering shader pass
	Unigine::Math::Vec3 last_paint_position{};			// Previous stroke position for spacing calculation

	Unigine::LandscapeLayerMapPtr landscape_layer_map = nullptr;	// Target landscape for painting
	Unigine::TexturePtr brush_texture = nullptr;		// Brush pattern texture
	Unigine::TexturePtr brush_mask = nullptr;			// Brush alpha mask texture

	float brush_size{1.0f};								// Brush diameter in world units
	float brush_spacing{0.2f};							// Stroke spacing as ratio of size
	float brush_angle{0.0f};							// Brush rotation in degrees
	Unigine::Math::vec4 brush_color{1.f, 1.f, 1.f, 1.f};	// Color multiplier for brush
	float brush_opacity{1.0f};							// Brush opacity factor

	// Parameters captured per async draw operation for deferred shader execution.
	struct DrawData
	{
		Unigine::TexturePtr brush_texture = nullptr;	// Brush pattern for this stroke
		Unigine::TexturePtr brush_mask = nullptr;		// Alpha mask for this stroke
		float brush_angle = 0.f;						// Rotation angle for this stroke
		float brush_opacity = 0.f;						// Opacity for this stroke
		Unigine::Math::vec4 brush_color{};				// Color for this stroke

		DrawData(Unigine::TexturePtr brush_texture, Unigine::TexturePtr brush_mask,
			float brush_angle, float brush_opacity, const Unigine::Math::vec4 &brush_color)
			: brush_texture{std::move(brush_texture)}
			, brush_mask{std::move(brush_mask)}
			, brush_angle{brush_angle}
			, brush_opacity{brush_opacity}
			, brush_color{brush_color}
		{}
	};

	Unigine::HashMap<int, DrawData> per_operation_draw_data;	// Pending draw operations by ID
	Unigine::EventConnection callback_handle{};					// Handle for texture draw event

	Unigine::ObjectMeshClutterPtr clutter = nullptr;			// Clutter to refresh after painting
};
