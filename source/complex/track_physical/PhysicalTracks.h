#pragma once
#include <UnigineComponentSystem.h>

#include "../../utils/Spline.h"

class PhysicalTracks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PhysicalTracks, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Switch, side, 0, "left,right");
	PROP_PARAM(Node, track_node);
	PROP_PARAM(Float, spacing, 1.0f);

private:
	Unigine::ObjectMeshStaticPtr mesh_static = nullptr;
	Unigine::Vector<Unigine::NodePtr> nodes;
	SplineNode spline;

private:
	void init();
	void shutdown();
};
