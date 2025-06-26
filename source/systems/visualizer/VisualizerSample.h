#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include "VisualizerUsage.h"

#include <UnigineComponentSystem.h>

class VisualizerSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(VisualizerSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information visualizer sample and enables it's 2D components");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	PROP_PARAM(Node, visualizer_usage_node, "Visualizer usage node");
	void init();
	void shutdown();
	VisualizerUsage* visualizer_usage = nullptr;
	SampleDescriptionWindow window;
};
