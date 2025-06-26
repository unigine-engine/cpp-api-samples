#pragma once
#include <UnigineComponentSystem.h>
#include "../../utils/Spline.h"

class NonPhysicalTracks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NonPhysicalTracks, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Switch, side, 0, "left,right");
	PROP_PARAM(Node, track_node);
	PROP_PARAM(Float, spacing, 1.0f);

	void setOffset(float offset);

private:
	Unigine::ObjectMeshStaticPtr mesh_static = nullptr;
	Unigine::ObjectMeshClusterPtr cluster; // cluster of objects that placed along spline
	SplineNode spline;
	bool is_initialized = false;
	float pos_offset = 0;

private:
	void init();
	void update();
	void shutdown();
};
