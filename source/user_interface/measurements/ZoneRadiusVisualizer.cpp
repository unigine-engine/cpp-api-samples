#include "ZoneRadiusVisualizer.h"

#include "../../utils/DistanceVisualizer.h"

using namespace Unigine;
using namespace Unigine::Math;

REGISTER_COMPONENT(ZoneRadiusVisualizer);

void ZoneRadiusVisualizer::update()
{
	if (zone_radius <= 0.0f)
		return;

	// A white ring plus a labeled radius line, redrawn each frame at the node's
	// current position.
	Vec3 position = node->getWorldPosition();
	DistanceVisualizer::renderZoneCircle(position, zone_radius);
	DistanceVisualizer::renderZoneRadius(position, zone_radius);
}
