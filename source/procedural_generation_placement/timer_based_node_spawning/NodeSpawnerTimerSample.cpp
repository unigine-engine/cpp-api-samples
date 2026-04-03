// Sample UI for timed node spawning. Displays a sphere that transitions from red
// to green as the spawn timer progresses. The sphere color provides visual feedback
// of time remaining until next spawn.

#include "NodeSpawnerTimerSample.h"

#include "NodeSpawnerTimer.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(NodeSpawnerTimerSample);

// Visualizer is enabled and spawn rate slider is created.
void NodeSpawnerTimerSample::init()
{
	Visualizer::setEnabled(true);
	sample_description_window.createWindow();

	spawner = getComponent<NodeSpawnerTimer>(timed_spawner);
	spawner->spawn_rate = 5.0f;
	auto gui = Gui::getCurrent();
	// Slider allows runtime adjustment of spawn interval
	sample_description_window.addFloatParameter("Spawn Frequency", "spawn frequency", 2, 0.1, 20,
		[this](float v) { spawner->spawn_rate = v; });
}

// Timer visualization sphere is rendered each frame with interpolated color.
void NodeSpawnerTimerSample::update()
{
	// Color interpolates from red (timer at 0) to green (timer at spawn_rate)
	vec4 color = lerp(vec4_red, vec4_green,
		inverseLerp(0, spawner->spawn_rate, spawner->getTimer()));
	color.w = 0.5f; // transparent
	Visualizer::renderSolidSphere(1.0f, timed_spawner->getWorldTransform(), color);
}

// Visualizer is disabled and UI window is cleaned up.
void NodeSpawnerTimerSample::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}
