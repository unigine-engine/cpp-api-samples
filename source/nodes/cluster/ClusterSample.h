// Interactive ObjectMeshCluster demonstration. Clicking on the ground adds
// instances at the clicked position; clicking on existing instances removes them.
// Demonstrates spatial tree updates required after modifying cluster contents.

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

	// Reference to the ObjectMeshCluster node to manipulate
	PROP_PARAM(Node, clusterNode);
	// Intersection mask for raycasting (determines which objects are hit)
	PROP_PARAM(Mask, intersection_mask, 1)

private:
	// Cached cluster pointer for efficient access during updates
	Unigine::ObjectMeshClusterPtr cluster;
	// Reusable intersection result object to avoid per-frame allocations
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
	// Sample window showing instance count
	SampleDescriptionWindow sample_description_window;

private:
	void init();
	void update();
	void shutdown();

	void init_gui();
	// Refreshes the displayed mesh count after add/remove operations
	void update_gui();
	void shutdown_gui();
};
