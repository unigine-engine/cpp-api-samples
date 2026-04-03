// Converts terrain height values to albedo colors using a gradient texture.
// Height range is mapped to gradient coordinates for colorization.
// Operates on entire landscape layer map using async texture draw.

#pragma once
#include <UnigineComponentSystem.h>

// Applies gradient-based colorization to terrain based on height values.
class LandscapeHeightToAlbedo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LandscapeHeightToAlbedo, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	void enable();		// Connects async texture draw callback
	void disable();		// Disconnects async texture draw callback

	void setTarget(const Unigine::LandscapeLayerMapPtr &in_landscape_layer_map) { landscape_layer_map = in_landscape_layer_map; }	// Sets target landscape

	void setGradient(const Unigine::TexturePtr &in_gradient) { gradient = in_gradient; }	// Sets color gradient texture
	void setMinHeight(float in_min_height) { min_height = in_min_height; }					// Sets minimum height for mapping
	void setMaxHeight(float in_max_height) { max_height = in_max_height; }					// Sets maximum height for mapping

	void run();		// Starts async height-to-albedo conversion

private:
	void init();
	// Callback invoked when async texture draw buffer is ready
	void texture_draw_callback(const Unigine::UGUID &guid, int id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask);

	Unigine::LandscapeLayerMapPtr landscape_layer_map;		// Target landscape for conversion
	Unigine::MaterialPtr material;							// Shader material for height-to-albedo conversion

	Unigine::TexturePtr gradient;		// Color gradient texture
	float min_height{ 0.0f };			// Height mapped to gradient start
	float max_height{ 1.0f };			// Height mapped to gradient end

	// Parameters captured per async draw operation.
	struct DrawData
	{
		Unigine::TexturePtr gradient;	// Gradient texture for this operation
		float min_height;				// Minimum height for this operation
		float max_height;				// Maximum height for this operation

		DrawData(
			const Unigine::TexturePtr &gradient,
			float min_height,
			float max_height)
			: gradient{ gradient }
			, min_height{ min_height }
			, max_height{ max_height }
		{ }
	};
	Unigine::HashMap<int, DrawData> per_operation_draw_data;	// Pending operations by ID

	Unigine::EventConnection callback_handle;	// Handle for texture draw event
};
