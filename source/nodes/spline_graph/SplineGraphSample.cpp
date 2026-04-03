// Demonstrates SplineGraph for creating and traversing smooth curves in 3D space.
// Points and segments are created procedurally, then visualized with tangent vectors.
// A mesh object follows the spline path using calcSegmentPoint() for smooth animation.

#include <UnigineComponentSystem.h>
#include <UnigineMathLibCommon.h>
#include <UnigineSplineGraph.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

namespace
{
	// Generate random vec3 within min/max bounds
	vec3 random_vec3(const vec3 &from, const vec3 &to)
	{
		return {
			Game::getRandomFloat(from.x, to.x),
			Game::getRandomFloat(from.y, to.y),
			Game::getRandomFloat(from.z, to.z)
		};
	}

	// Generate random vec3 within symmetric bounds
	vec3 random_vec3(const vec3 &size)
	{
		return random_vec3(- size * .5f, size * .5f);
	}
}

class SplineGraphSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(SplineGraphSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// Mesh file to use for the moving object
	PROP_PARAM(File, mesh_file);

private:
	void init()
	{
		Unigine::String mesh_path = mesh_file.get();
		if (mesh_path.size() <= 0)
		{
			Unigine::Log::error("SplineGraphSample::init(): Mesh File param is empty!\n");
		}

		// Create empty spline graph container
		spline_graph = SplineGraph::create();
		// Create mesh that will follow the spline path
		box = ObjectMeshStatic::create();
		box->setMeshPath(mesh_path);

		generate_graph();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void update()
	{
		render_graph();
		move_box_along_graph();
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	void generate_graph()
	{
		int num_points = 4;
		vec3 size = { 3.f, 3.f, 2.f };
		Vec3 offset { 0.f, 3.f, size.z * .5f + 1.f };

		// Initialize random seed from current time
		Game::setSeed(int(Time::get()));

		// Create random control points within the defined volume
		for (int i = 0; i < num_points; i += 1)
			spline_graph->addPoint(Vec3(::random_vec3(size)) + offset);

		// Connect points with cubic Bezier segments (closed loop)
		for (int i = 0; i < num_points; i += 1)
		{
			// Window of 4 points for Catmull-Rom-like tangent calculation
			int index_window[4] = {
				(num_points + (i - 1) % num_points) % num_points,
				i,
				(i + 1) % num_points,
				(i + 2) % num_points,
			};

			// Calculate smooth tangents: direction from previous to next point
			vec3 from_tangent = vec3(normalize(spline_graph->getPoint(index_window[2]) - spline_graph->getPoint(index_window[0])));
			vec3 to_tangent = - vec3(normalize(spline_graph->getPoint(index_window[3]) - spline_graph->getPoint(index_window[1])));

			// Create segment with tangents and up vectors
			spline_graph->addSegment(index_window[1], from_tangent, vec3_up, index_window[2], to_tangent, vec3_up);
		}
	}

	void render_graph()
	{
		const int num_segments = 32;  // Subdivision level for visualization

		// Draw control points as white dots
		for (int i = 0; i < spline_graph->getNumPoints(); i++)
		{
			Vec3 p = Vec3(spline_graph->getPoint(i));
			Visualizer::renderPoint3D(p, 0.05f, vec4_white);
		}

		// Draw each spline segment with tangent vectors
		for (int i = 0; i < spline_graph->getNumSegments(); i++)
		{
			vec4 tangent_color = vec4(vec3(89, 204, 249) / 255.f, 1.f);

			// Get segment endpoints and their tangents
			Vec3 start_point	=	Vec3(spline_graph->getSegmentStartPoint(i));
			Vec3 start_tangent	=	Vec3(spline_graph->getSegmentStartTangent(i));
			Vec3 end_point		=	Vec3(spline_graph->getSegmentEndPoint(i));
			Vec3 end_tangent	=	Vec3(spline_graph->getSegmentEndTangent(i));

			// Visualize tangent vectors at control points
			Visualizer::renderVector(start_point, start_point + start_tangent, tangent_color, 0.15f);
			Visualizer::renderVector(end_point, end_point + end_tangent, tangent_color, 0.15f);

			// Draw the curve as a series of line segments
			for (int j = 0; j < num_segments; j++)
			{
				// calcSegmentPoint() returns interpolated position along segment (t: 0-1)
				Vec3 p0 = Vec3(spline_graph->calcSegmentPoint(i, float(j) / num_segments));
				Vec3 p1 = Vec3(spline_graph->calcSegmentPoint(i, float(j + 1) / num_segments));
				Visualizer::renderLine3D(p0, p1, vec4_white);
			}
		}
	}

	void move_box_along_graph()
	{
		float time = Game::getTime();

		// Calculate position along the entire spline (loops through segments)
		float t = time - Math::floor(time / spline_graph->getNumSegments()) * spline_graph->getNumSegments();
		// Extract segment index and local parameter
		int segment_id = int(t);
		t -= float(segment_id);

		// Get interpolated position, tangent (direction), and up vector
		Vec3 p = Vec3(spline_graph->calcSegmentPoint(segment_id, t));
		vec3 direction = spline_graph->calcSegmentTangent(segment_id, t);
		vec3 up = spline_graph->calcSegmentUpVector(segment_id, t);

		// Apply position and orientation to the moving mesh
		box->setWorldPosition(p);
		box->setWorldDirection(direction, up);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	// Procedural spline path
	SplineGraphPtr spline_graph;
	// Mesh following the spline
	ObjectMeshStaticPtr box;
};

REGISTER_COMPONENT(SplineGraphSample);
