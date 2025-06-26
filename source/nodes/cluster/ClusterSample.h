#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class ClusterSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ClusterSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, clusterNode);
	PROP_PARAM(Mask, intersection_mask, 1)

private:
	Unigine::ObjectMeshClusterPtr cluster;
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
	SampleDescriptionWindow sample_description_window;

private:
	void init();
	void update();
	void shutdown();

	void init_gui();
	void update_gui();
	void shutdown_gui();
};