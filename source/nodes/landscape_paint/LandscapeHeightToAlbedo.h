#pragma once
#include <UnigineComponentSystem.h>

class LandscapeHeightToAlbedo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapeHeightToAlbedo, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	void enable();
	void disable();

	void setTarget(const Unigine::LandscapeLayerMapPtr &in_landscape_layer_map) { landscape_layer_map = in_landscape_layer_map; }

	void setGradient(const Unigine::TexturePtr &in_gradient) { gradient = in_gradient; }
	void setMinHeight(float in_min_height) { min_height = in_min_height; }
	void setMaxHeight(float in_max_height) { max_height = in_max_height; }

	void run();

private:
	void init();
	void texture_draw_callback(const Unigine::UGUID &guid, int id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask);

	Unigine::LandscapeLayerMapPtr landscape_layer_map;
	Unigine::MaterialPtr material;

	Unigine::TexturePtr gradient;
	float min_height{ 0.0f };
	float max_height{ 1.0f };

	struct DrawData
	{
		Unigine::TexturePtr gradient;
		float min_height;
		float max_height;

		DrawData(
			const Unigine::TexturePtr &gradient,
			float min_height,
			float max_height)
			: gradient{ gradient }
			, min_height{ min_height }
			, max_height{ max_height }
		{ }
	};
	Unigine::HashMap<int, DrawData> per_operation_draw_data;

	Unigine::EventConnection callback_handle;
};
