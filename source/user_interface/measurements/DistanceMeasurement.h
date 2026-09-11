// Draws a colored distance "ruler" from this node to each target node every
// frame: red when close, green at green_distance and beyond. Attach it to any
// node, assign the targets and enable the Visualizer (the sample does that) to
// see the rulers. Reusable on its own, no sample code required.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineNode.h>
#include <UnigineVector.h>

class DistanceMeasurement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DistanceMeasurement, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Nodes whose distance to this node is measured and drawn.
	PROP_ARRAY(Node, targets);
	// Distance (in units) at which a ruler becomes fully green.
	PROP_PARAM(Float, green_distance, 25.0f, "", "Distance mapped to green");

private:
	void init();
	void update();

	// This node plus its sub-tree: the "from" end of every ruler, also excluded
	// from the surface-clipping raycast. Collected once (hierarchy is static).
	Unigine::Vector<Unigine::NodePtr> own_hierarchy;
};
