// Sample UI for timed node spawning with visual timer feedback.
// Renders a color-interpolated sphere that transitions from red
// to green as the spawn timer progresses toward the next spawn.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class NodeSpawnerTimer;

// Provides spawn rate slider and renders timer visualization sphere.
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
	// Reference to the spawner component for accessing timer state
	NodeSpawnerTimer *spawner = nullptr;
	SampleDescriptionWindow sample_description_window;
};
