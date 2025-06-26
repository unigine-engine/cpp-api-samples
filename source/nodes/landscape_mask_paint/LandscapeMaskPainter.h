#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineTextures.h>

#include <utility>

class LandscapeMaskPainter : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapeMaskPainter, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	PROP_PARAM(Node, clutter_param);

	void enable();
	void disable();

	void setTarget(const Unigine::LandscapeLayerMapPtr &in_landscape_layer_map)
	{
		landscape_layer_map = in_landscape_layer_map;
	}

	void setBrushTexture(const Unigine::TexturePtr &in_brush_texture)
	{
		brush_texture = in_brush_texture;
	}

	void setBrushMask(const Unigine::TexturePtr &in_brush_texture)
	{
		brush_mask = in_brush_texture;
	}

	void setBrushSize(float in_brush_size) { brush_size = in_brush_size; }

	void setBrushSpacing(float in_brush_spacing) { brush_spacing = in_brush_spacing; }

	void setBrushAngle(float in_brush_angle) { brush_angle = in_brush_angle; }

	void setBrushOpacity(float in_brush_opacity) { brush_opacity = in_brush_opacity; }

	void setBrushColor(const Unigine::Math::vec4 &in_brush_color) { brush_color = in_brush_color; }

	void paintAt(const Unigine::Math::Vec3 &world_position);

private:
	void init();
	void texture_draw_callback(const Unigine::UGUID &guid, int id,
		const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord,
		int data_mask);


private:
	Unigine::MaterialPtr brush_material = nullptr;
	Unigine::Math::Vec3 last_paint_position{};

	Unigine::LandscapeLayerMapPtr landscape_layer_map = nullptr;
	Unigine::TexturePtr brush_texture = nullptr;
	Unigine::TexturePtr brush_mask = nullptr;

	float brush_size{1.0f};
	float brush_spacing{0.2f};
	float brush_angle{0.0f};
	Unigine::Math::vec4 brush_color{1.f, 1.f, 1.f, 1.f};
	float brush_opacity{1.0f};

	struct DrawData
	{
		Unigine::TexturePtr brush_texture = nullptr;
		Unigine::TexturePtr brush_mask = nullptr;
		float brush_angle = 0.f;
		float brush_opacity = 0.f;
		Unigine::Math::vec4 brush_color{};

		DrawData(Unigine::TexturePtr brush_texture, Unigine::TexturePtr brush_mask,
			float brush_angle, float brush_opacity, const Unigine::Math::vec4 &brush_color)
			: brush_texture{std::move(brush_texture)}
			, brush_mask{std::move(brush_mask)}
			, brush_angle{brush_angle}
			, brush_opacity{brush_opacity}
			, brush_color{brush_color}
		{}
	};

	Unigine::HashMap<int, DrawData> per_operation_draw_data;
	Unigine::EventConnection callback_handle{};

	Unigine::ObjectMeshClutterPtr clutter = nullptr;
};
