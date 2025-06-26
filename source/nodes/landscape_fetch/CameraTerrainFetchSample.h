#pragma once
#include <UnigineVector.h>
#include <UniginePlayers.h>
#include <UnigineComponentSystem.h>


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

	Unigine::PlayerPtr main_player;
	Unigine::LandscapeFetchPtr fetch;
	bool visualizer_enabled = false;
	int num_masks = 4;
};
