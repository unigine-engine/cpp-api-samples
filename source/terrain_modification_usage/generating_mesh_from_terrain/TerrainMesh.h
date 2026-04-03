// Generates a dynamic mesh from terrain height data using async LandscapeFetch.
// Grid of sample positions is fetched asynchronously with configurable concurrency.
// Resulting mesh vertices contain terrain height and albedo color from landscape.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

// Fetches terrain data and generates a dynamic mesh representation.
class TerrainMesh : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TerrainMesh, Unigine::ComponentBase);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Material, material_prop, "Material");	// Material applied to generated mesh

	void setResolution(const Unigine::Math::ivec2 &in_resolution) { resolution = in_resolution; }	// Sets vertex grid resolution
	void setMaxFetchers(int in_max_fetchers) { max_fetchers = in_max_fetchers; }					// Sets concurrent fetch limit

	void setDrawBoundingBox(bool in_draw_bounding_box) { draw_bounding_box = in_draw_bounding_box; }	// Enables bounding box visualization
	void setDrawWireframe(bool in_draw_wireframe) { draw_wireframe = in_draw_wireframe; }			// Enables wireframe visualization

	int getTotalFetchPositions() const { return fetch_positions.size(); }		// Returns total sample points count
	int getCompletedFetchPositions() const { return fishined_fetch_count; }		// Returns completed fetch count

	void run();		// Starts async terrain fetching operation

	static constexpr int INVALID_FETCH_HEIGHT = -1000000000;	// Height value returned by LandscapeFetch when no landscape

private:
	void update();
	void generate_mesh();		// Builds mesh from fetched terrain data

	Unigine::Math::ivec2 resolution{64, 64};	// Grid resolution in X and Y
	int max_fetchers{64};						// Maximum concurrent async fetches

	Unigine::Vector<Unigine::Math::Vec2> fetch_positions;	// World positions to sample
	int next_fetch_index{0};								// Index of next position to fetch

	// Single async terrain fetcher with output index.
	struct Fetcher
	{
		Unigine::LandscapeFetchPtr landscape_fetch;		// Async fetch object
		int out_data_index{0};							// Output index in fetch_data array
	};
	Unigine::Vector<Fetcher> fetchers;		// Pool of concurrent fetchers

	// Height and color data retrieved from terrain.
	struct FetchData
	{
		float height;					// Terrain height at sample point
		Unigine::Math::vec4 albedo;		// Terrain albedo color at sample point
	};
	Unigine::Vector<FetchData> fetch_data;	// Results array for all sample positions
	int fishined_fetch_count{0};			// Number of completed fetches

	Unigine::ObjectMeshDynamicPtr mesh_dynamic;		// Generated mesh object

	// State machine for async fetch operation.
	enum class State
	{
		IDLE,		// Ready to start new fetch
		FETCHING	// Async fetches in progress
	};
	State state{State::IDLE};

	bool draw_bounding_box{true};		// Visualizer bounding box flag
	bool draw_wireframe{false};			// Visualizer wireframe flag
};
