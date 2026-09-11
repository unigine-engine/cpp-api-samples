// Draws a flat zone ring plus a labeled radius line around this node's position
// every frame. Attach it to any node you want to mark with a zone and enable
// the Visualizer (the sample does that) to see it. Reusable on its own.

#pragma once

#include <UnigineComponentSystem.h>

class ZoneRadiusVisualizer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ZoneRadiusVisualizer, Unigine::ComponentBase);

	COMPONENT_UPDATE(update);

	// Radius of the zone circle drawn around this node.
	PROP_PARAM(Float, zone_radius, 3.0f, "", "Radius of the drawn zone");

private:
	void update();
};
