// Performs GPU-based height slicing on landscape terrain data. Reads height from
// a source LandscapeLayerMap and writes modified height/albedo to a target layer.
// Uses async texture draw callbacks and a state machine for multi-frame processing.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

class HeightSlicer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HeightSlicer, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Connects the texture draw callback to Landscape system
	void enable();
	// Disconnects all callbacks
	void disable();

	void setSourceLmap(const Unigine::LandscapeLayerMapPtr &in_source_lmap);
	void setTargetLmap(const Unigine::LandscapeLayerMapPtr &in_target_lmap) { target_lmap = in_target_lmap; }
	// Sets normalized height threshold (0.0 to 1.0)
	void setSliceHeight(float in_slice_height) { slice_height = in_slice_height; }

	// Starts the slicing operation (non-blocking, state machine driven)
	void run();

private:
	void init();
	void update();
	// Callback invoked by Landscape::asyncTextureDraw when render is ready
	void texture_draw_callback(const Unigine::UGUID &guid,
			int id, const Unigine::LandscapeTexturesPtr &buffer,
			const Unigine::Math::ivec2 &coord, int data_mask);
	// List of landscape maps to render from
	Unigine::Vector<Unigine::LandscapeLayerMapPtr> maps;
	// Cached height texture from source layer
	Unigine::TexturePtr source_lmap_height;
	Unigine::LandscapeLayerMapPtr source_lmap;
	// Intermediate buffer for landscape texture operations
	Unigine::LandscapeTexturesPtr buffers;
	Unigine::LandscapeLayerMapPtr target_lmap;
	// Normalized slice threshold (multiplied by max height at runtime)
	float slice_height{ 0.5f };
	// Flag to track if source height has been rendered
	bool buffers_rendered = false;
	

	// State machine for multi-frame async processing
	enum class State
	{
		IDLE,
		BEGIN_FETCH_HEIGHT,  // Start fetching source height data
		FETCHING_HEIGHT,     // Waiting for height render to complete
		BEGIN_DRAW,          // Start async draw to target
		DRAWING              // Waiting for draw callback
	};

	State state = State::IDLE;

	// Material containing the slice shader
	Unigine::MaterialPtr slice_material;
};
