#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

class HeightSlicer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HeightSlicer, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	void enable();
	void disable();

	void setSourceLmap(const Unigine::LandscapeLayerMapPtr &in_source_lmap);
	void setTargetLmap(const Unigine::LandscapeLayerMapPtr &in_target_lmap) { target_lmap = in_target_lmap; }
	void setSliceHeight(float in_slice_height) { slice_height = in_slice_height; }

	void run();

private:
	void init();
	void update();
	void texture_draw_callback(const Unigine::UGUID &guid,
			int id, const Unigine::LandscapeTexturesPtr &buffer,
			const Unigine::Math::ivec2 &coord, int data_mask);
	Unigine::Vector<Unigine::LandscapeLayerMapPtr> maps;
	Unigine::TexturePtr source_lmap_height;
	Unigine::LandscapeLayerMapPtr source_lmap;
	Unigine::LandscapeTexturesPtr buffers;
	Unigine::LandscapeLayerMapPtr target_lmap;
	float slice_height{ 0.5f };
	bool buffers_rendered = false;
	

	enum class State
	{
		IDLE,
		BEGIN_FETCH_HEIGHT,
		FETCHING_HEIGHT,
		BEGIN_DRAW,
		DRAWING
	};

	State state = State::IDLE;

	Unigine::MaterialPtr slice_material;
};
