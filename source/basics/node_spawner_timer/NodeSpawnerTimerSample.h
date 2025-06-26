#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class NodeSpawnerTimer;

class NodeSpawnerTimerSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeSpawnerTimerSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, timed_spawner);

private:
	void init();
	void update();
	void shutdown();

private:
	NodeSpawnerTimer *spawner = nullptr;
	SampleDescriptionWindow sample_description_window;
};
