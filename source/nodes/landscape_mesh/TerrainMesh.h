#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

class TerrainMesh : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TerrainMesh, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Material, material_prop, "Material");

	void setResolution(const Unigine::Math::ivec2 &in_resolution) { resolution = in_resolution; }
	void setMaxFetchers(int in_max_fetchers){ max_fetchers = in_max_fetchers; }

	void setDrawBoundingBox(bool in_draw_bounding_box) { draw_bounding_box = in_draw_bounding_box; }
	void setDrawWireframe(bool in_draw_wireframe) { draw_wireframe = in_draw_wireframe; }

	int getTotalFetchPositions() const { return fetch_positions.size(); }
	int getCompletedFetchPositions() const { return fishined_fetch_count; }

	void run();
private:
	void update();

	void generate_mesh();

	Unigine::Math::ivec2 resolution{64, 64};
	int max_fetchers{64};

	Unigine::Vector<Unigine::Math::Vec2> fetch_positions;
	int next_fetch_index{0};

	struct Fetcher
	{
		Unigine::LandscapeFetchPtr landscape_fetch;
		int out_data_index{0};
	};
	Unigine::Vector<Fetcher> fetchers;

	struct FetchData
	{
		float height;
		Unigine::Math::vec4 albedo;
	};
	Unigine::Vector<FetchData> fetch_data;
	int fishined_fetch_count{0};

	Unigine::ObjectMeshDynamicPtr mesh_dynamic;

	enum class State
	{
		IDLE,
		FETCHING
	};
	State state{State::IDLE};

	bool draw_bounding_box{true};
	bool draw_wireframe{false};
};