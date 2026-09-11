#include "DistanceVisualizer.h"

#include <UnigineVisualizer.h>
#include <UnigineString.h>
#include <UnigineWorld.h>
#include <UnigineObjects.h>
#include <UnigineGame.h>
#include <UniginePlayers.h>

using namespace Unigine;
using namespace Unigine::Math;

namespace DistanceVisualizer
{

vec4 redGreenGradient(float t)
{
	t = saturate(t);
	if (t < 0.5f)
		return vec4(1.0f, t * 2.0f, 0.0f, 1.0f);
	return vec4(2.0f - t * 2.0f, 1.0f, 0.0f, 1.0f);
}

// --- helpers used by setupDistanceVector ---------------------------------

// Combined world bound box of a list of nodes (skipping null entries).
static WorldBoundBox expandBoundBox(const Vector<NodePtr> &nodes)
{
	WorldBoundBox bbox;
	for (int i = 0; i < nodes.size(); i++)
		if (nodes[i])
			bbox.expand(nodes[i]->getWorldBoundBox());
	return bbox;
}

// Clips the ray to the nearest surface among `to.nodes`. Object::getIntersection
// is object-local, so the ray and the hit point are converted via its world
// transform.
static bool castEndpointRay(const DistanceVector::Endpoint &from, const DistanceVector::Endpoint &to,
	const ObjectIntersectionPtr &intersection)
{
	bool found = false;
	double best_distance = 0.0;
	Vec3 best_point = Vec3_zero;

	for (int i = 0; i < to.nodes.size(); i++)
	{
		ObjectPtr object = checked_ptr_cast<Object>(to.nodes[i]);
		if (!object)
			continue;

		Mat4 iworld = object->getIWorldTransform();
		Vec3 local_p0 = iworld * from.point;
		Vec3 local_p1 = iworld * to.point;
		if (object->getIntersection(local_p0, local_p1, to.intersection_mask, intersection, nullptr))
		{
			Vec3 point = object->getWorldTransform() * intersection->getPoint();
			double hit_distance = length(point - from.point);
			if (!found || hit_distance < best_distance)
			{
				found = true;
				best_distance = hit_distance;
				best_point = point;
			}
		}
	}

	if (found)
		intersection->setPoint(best_point);   // hand the caller a WORLD point
	return found;
}

// --- public API ----------------------------------------------------------

Vector<NodePtr> collectHierarchy(const NodePtr &node)
{
	Vector<NodePtr> nodes;
	if (node)
		node->getHierarchy(nodes);   // built-in: the node plus its whole sub-tree
	return nodes;
}

void setupDistanceVector(DistanceVector &vector)
{
	// Reused across calls to avoid allocating an intersection object per frame.
	static ObjectIntersectionPtr intersection = ObjectIntersection::create();

	// If only nodes are given, take the endpoint as the center of their bbox.
	if (vector.from.nodes.size() && compare(vector.from.point, Vec3_zero))
		vector.from.point = expandBoundBox(vector.from.nodes).getCenter();
	if (vector.to.nodes.size() && compare(vector.to.point, Vec3_zero))
		vector.to.point = expandBoundBox(vector.to.nodes).getCenter();

	// Centers, kept so the surface clips below stay between them.
	Vec3 from_center = vector.from.point;
	Vec3 to_center = vector.to.point;

	// Pull each endpoint to the surface it faces, so the arrows touch the meshes.
	if (castEndpointRay(vector.from, vector.to, intersection))
		vector.to.point = intersection->getPoint();
	if (castEndpointRay(vector.to, vector.from, intersection))
		vector.from.point = intersection->getPoint();

	// If the objects touch/overlap the clips cross and the ruler inverts; fall
	// back to the centers so the length stays meaningful instead of ~0.
	if (dot(vector.to.point - vector.from.point, to_center - from_center) <= 0.0)
	{
		vector.from.point = from_center;
		vector.to.point = to_center;
	}
}

void renderDistanceVector(const DistanceVector &vector)
{
	float length = float(distance(vector.from.point, vector.to.point));
	// renderVector wants the head as a fraction of the whole length (guard /0).
	float safe_length = compare(length, 0.0f) ? 1.0f : length;

	// An arrow head at each end gives the symmetric "ruler" look.
	Visualizer::renderVector(vector.from.point, vector.to.point, vector.color,
		vector.to.cone_length / safe_length, vector.screen_space, vector.duration, vector.depth_test);
	Visualizer::renderVector(vector.to.point, vector.from.point, vector.color,
		vector.from.cone_length / safe_length, vector.screen_space, vector.duration, vector.depth_test);

	// Solid shaft between the heads (skip it when they already overlap).
	float height = length - vector.from.cone_length - vector.to.cone_length;
	if (height > 0.0f)
	{
		vec3 direction = vec3(normalizeValid(vector.to.point - vector.from.point));
		Vec3 position = vector.from.point + Vec3(direction) * (vector.from.cone_length + height * 0.5f);
		Mat4 transform = Mat4(rotationFromDir(direction) * quat(vec3_right, 90.0f), position);
		Visualizer::renderSolidCylinder(vector.cylinder_radius, height, transform, vector.color,
			vector.duration, vector.depth_test);
	}

	// Distance label lifted above the middle so it does not sink into the shaft.
	Vec3 middle = vector.from.point + (vector.to.point - vector.from.point) * 0.5f;
	float z_offset = 0.1f + min(0.1f * length, 1.0f);
	Vec3 label_position = middle + Vec3_up * z_offset;

	StringStack<> text = vector.label_message
		? StringStack<>(vector.label_message)
		: String::format(vector.label_format, length);

	Visualizer::renderMessage3D(label_position, vec3_zero, text.get(), vector.label_color,
		vector.label_outline, vector.label_size, vector.duration);
}

void renderZoneRadius(const Vec3 &position, float radius)
{
	DistanceVector vector;
	vec3 direction = vec3_right;

	// Turn the radius toward the camera (flattened to the ground plane) so the
	// line and its label do not end up behind the node.
	PlayerPtr player = Game::getPlayer();
	if (player)
	{
		Vec3 player_position = player->getWorldPosition();
		vec3 flat_direction = vec3(normalizeValid((player_position - position) * Vec3(1.0f, 1.0f, 0.0f)));
		direction = quat(vec3_up, 60.0f) * flat_direction;
	}

	vector.from.point = position;
	vector.from.intersection_mask = 0;
	vector.to.point = position + Vec3(direction) * radius;
	vector.to.intersection_mask = 0;
	vector.color = vec4(1.0f, 1.0f, 1.0f, 0.4f);
	vector.label_color = vec4(1.0f, 1.0f, 1.0f, 0.8f);

	renderDistanceVector(vector);
}

void renderZoneCircle(const Vec3 &center, float radius)
{
	// Identity rotation keeps the circle in the XY plane (lying flat on the
	// ground); lift it a hair to avoid z-fighting with the floor.
	Mat4 transform = Mat4(quat_identity, center + Vec3_up * 0.02f);
	Visualizer::renderCircle(radius, transform, vec4(1.0f, 1.0f, 1.0f, 0.6f));
}

void renderDistanceVectorEx(const DistanceVectorEx &vector_ex)
{
	DistanceVector vector = vector_ex.vec;
	setupDistanceVector(vector);

	float d = float(distance(vector.to.point, vector.from.point));
	float t = saturate((d - vector_ex.threshold) / vector_ex.min_visible_d);

	// Full alpha until min_visible_d, linear fade to 0 at max_visible_d.
	float alpha = 1.0f;
	if (d > vector_ex.min_visible_d && d <= vector_ex.max_visible_d)
		alpha = 1.0f - (d - vector_ex.min_visible_d) / (vector_ex.max_visible_d - vector_ex.min_visible_d);
	if (d > vector_ex.max_visible_d)
		alpha = 0.0f;
	alpha *= vector_ex.time;

	vector.color = redGreenGradient(t);
	vector.color.w = alpha;
	vector.label_color = vector.color;
	vector.label_color.w *= 1.5f;

	renderDistanceVector(vector);
}

} // namespace DistanceVisualizer
