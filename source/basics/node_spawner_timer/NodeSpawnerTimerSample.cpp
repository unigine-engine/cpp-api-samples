#include "NodeSpawnerTimerSample.h"

#include "NodeSpawnerTimer.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(NodeSpawnerTimerSample);

void NodeSpawnerTimerSample::init()
{
	Visualizer::setEnabled(true);
	sample_description_window.createWindow();

	spawner = getComponent<NodeSpawnerTimer>(timed_spawner);
	spawner->spawn_rate = 5.0f;
	auto gui = Gui::getCurrent();
	sample_description_window.addFloatParameter("Spawn Frequency", "spawn frequency", 2, 0.1, 20,
		[this](float v) { spawner->spawn_rate = v; });
}

void NodeSpawnerTimerSample::update()
{
	vec4 color = lerp(vec4_red, vec4_green,
		inverseLerp(0, spawner->spawn_rate, spawner->getTimer()));
	color.w = 0.5f; // transparent
	Visualizer::renderSolidSphere(1.0f, timed_spawner->getWorldTransform(), color);
}

void NodeSpawnerTimerSample::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}
