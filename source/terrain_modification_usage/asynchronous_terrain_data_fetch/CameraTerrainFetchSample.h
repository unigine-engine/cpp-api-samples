// Demonstrates asynchronous terrain data fetching using LandscapeFetch.
// Raycasts from camera through mouse cursor to terrain surface.
// Displays terrain height, normal, and mask values at intersection point.

#pragma once
#include <UnigineVector.h>
#include <UniginePlayers.h>
#include <UnigineComponentSystem.h>

// Fetches and displays terrain data at mouse cursor position.
class CameraTerrainFetchSample : public Unigine::ComponentBase
{
public:
	COMPONENT(CameraTerrainFetchSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_NAME("CameraTerrainFetchSample")
	PROP_AUTOSAVE(0)

private:
	void init();
	void update();
	void shutdown();

	Unigine::PlayerPtr main_player;				// Camera player node this component is attached to
	Unigine::LandscapeFetchPtr fetch;			// Async terrain data fetcher
	bool visualizer_enabled = false;			// Original visualizer state for restoration
	int num_masks = 4;							// Number of terrain masks to query
};
