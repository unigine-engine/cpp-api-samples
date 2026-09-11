// Distance "ruler" visualizer. Draws a measured distance between two world points
// as a double-headed arrow + cylinder shaft + 3D length label, optionally
// clipped to the surfaces of the measured objects and faded by distance.
//
// All functions render for a single frame, so call them every Update() while
// the Visualizer is enabled.

#pragma once

#include <UnigineMathLib.h>
#include <UnigineVector.h>
#include <UnigineNode.h>

// Distance "ruler" helpers, grouped so they don't leak into the global scope
// (same convention as GeometryGenerator / MarchingCubes in utils).
namespace DistanceVisualizer
{

// One measurement between two endpoints. An endpoint can be given either as an
// explicit point or as a list of nodes (then the point is the center of their
// combined bound box, see setupDistanceVector).
struct DistanceVector
{
	struct Endpoint
	{
		Unigine::Vector<Unigine::NodePtr> nodes;
		Unigine::Math::Vec3 point = Unigine::Math::Vec3_zero;
		int intersection_mask = 1;     // mask used when clipping to a surface
		float cone_length = 0.2f;      // length of this end's arrow head
	};

	Unigine::Math::vec4 color = Unigine::Math::vec4(1.0f, 1.0f, 1.0f, 0.6f);
	Unigine::Math::vec4 label_color = Unigine::Math::vec4_white;

	// When set, the label shows this text instead of the measured length.
	const char *label_message = nullptr;
	const char *label_format = "%.2f m";

	float cylinder_radius = 0.008f;
	float duration = 0.0f;
	bool screen_space = false;
	bool depth_test = true;
	int label_outline = 1;
	int label_size = 24;

	Endpoint from;
	Endpoint to;
};

// A DistanceVector that fades in and out with distance: fully visible up to
// min_visible_d, fading to invisible at max_visible_d. Color runs red -> green
// over the distance range.
struct DistanceVectorEx
{
	DistanceVector vec;
	float max_visible_d = 3.5f;
	float min_visible_d = 2.5f;
	float threshold = 0.2f;
	float time = 1.0f;     // extra alpha multiplier (e.g. for a global fade)
};

// Smoothly blends red (t = 0) through yellow to green (t = 1).
Unigine::Math::vec4 redGreenGradient(float t);

// Collects a node and its whole sub-tree into a flat list.
Unigine::Vector<Unigine::NodePtr> collectHierarchy(const Unigine::NodePtr &node);

// Resolves endpoint points from their node lists and clips each endpoint to the
// first surface hit on the way to the other end, so the arrows touch the
// objects instead of their centers. Safe to skip if you set points directly.
void setupDistanceVector(DistanceVector &vector);

// Draws the measurement (two arrow heads + shaft + distance label).
void renderDistanceVector(const DistanceVector &vector);

// Draws a single radius line of the given length out of `position`, turned
// toward the camera so its label stays readable, labeled with the radius.
void renderZoneRadius(const Unigine::Math::Vec3 &position, float radius);

// Draws a flat ring of the given radius around `center` (the circle lies in
// the XY plane, normal pointing up). In the rover demo the zone ring is a
// marker mesh with a material; here it is drawn directly with the Visualizer
// so the sample needs no extra content.
void renderZoneCircle(const Unigine::Math::Vec3 &center, float radius);

// Draws a DistanceVector with distance-based color and fade (see above).
void renderDistanceVectorEx(const DistanceVectorEx &vector);

} // namespace DistanceVisualizer
