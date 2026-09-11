#include "DistanceMeasurement.h"

#include "../../utils/DistanceVisualizer.h"

using namespace Unigine;
using namespace Unigine::Math;

REGISTER_COMPONENT(DistanceMeasurement);

void DistanceMeasurement::init()
{
	// The controlled node is the "from" end of every ruler; its hierarchy does
	// not change at runtime, so collect it once.
	own_hierarchy = DistanceVisualizer::collectHierarchy(node);
}

void DistanceMeasurement::update()
{
	if (own_hierarchy.size() == 0)
		return;

	for (int i = 0; i < targets.size(); i++)
	{
		NodePtr target = targets.get(i).get();
		// Targets can be deleted at runtime, so always check before use.
		if (!target)
			continue;

		// Let setupDistanceVector resolve the endpoints to the object surfaces
		// so the arrows touch the meshes instead of their centers.
		DistanceVisualizer::DistanceVector vector;
		vector.from.nodes = own_hierarchy;
		vector.to.nodes = DistanceVisualizer::collectHierarchy(target);
		DistanceVisualizer::setupDistanceVector(vector);

		// Color the ruler red (near) to green (far) by the measured distance.
		float length = float(distance(vector.from.point, vector.to.point));
		float t = green_distance > 0.0f ? saturate(length / green_distance) : 1.0f;
		vector.color = DistanceVisualizer::redGreenGradient(t);
		vector.color.w = 0.6f;
		vector.label_color = vector.color;
		vector.label_color.w = 1.0f;

		DistanceVisualizer::renderDistanceVector(vector);
	}
}
