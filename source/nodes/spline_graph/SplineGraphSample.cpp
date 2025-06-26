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
	vec3 random_vec3(const vec3 &from, const vec3 &to)
	{
		return {
			Game::getRandomFloat(from.x, to.x),
			Game::getRandomFloat(from.y, to.y),
			Game::getRandomFloat(from.z, to.z)
		};
	}

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

	PROP_PARAM(File, mesh_file);

private:
	void init()
	{
		Unigine::String mesh_path = mesh_file.get();
		if (mesh_path.size() <= 0)
		{
			Unigine::Log::error("SplineGraphSample::init(): Mesh File param is empty!\n");
		}

		spline_graph = SplineGraph::create();
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

		Game::setSeed(int(Time::get()));

		for (int i = 0; i < num_points; i += 1)
			spline_graph->addPoint(Vec3(::random_vec3(size)) + offset);

		for (int i = 0; i < num_points; i += 1)
		{
			int index_window[4] = {
				(num_points + (i - 1) % num_points) % num_points,
				i,
				(i + 1) % num_points,
				(i + 2) % num_points,
			};

			vec3 from_tangent = vec3(normalize(spline_graph->getPoint(index_window[2]) - spline_graph->getPoint(index_window[0])));
			vec3 to_tangent = - vec3(normalize(spline_graph->getPoint(index_window[3]) - spline_graph->getPoint(index_window[1])));

			spline_graph->addSegment(index_window[1], from_tangent, vec3_up, index_window[2], to_tangent, vec3_up);
		}
	}

	void render_graph()
	{
		const int num_segments = 32;
		for (int i = 0; i < spline_graph->getNumPoints(); i++)
		{
			Vec3 p = Vec3(spline_graph->getPoint(i));
			Visualizer::renderPoint3D(p, 0.05f, vec4_white);
		}

		for (int i = 0; i < spline_graph->getNumSegments(); i++)
		{
			vec4 tangent_color = vec4(vec3(89, 204, 249) / 255.f, 1.f);

			Vec3 start_point	=	Vec3(spline_graph->getSegmentStartPoint(i));
			Vec3 start_tangent	=	Vec3(spline_graph->getSegmentStartTangent(i));
			Vec3 end_point		=	Vec3(spline_graph->getSegmentEndPoint(i));
			Vec3 end_tangent	=	Vec3(spline_graph->getSegmentEndTangent(i));

			Visualizer::renderVector(start_point, start_point + start_tangent, tangent_color, 0.15f);
			Visualizer::renderVector(end_point, end_point + end_tangent, tangent_color, 0.15f);

			for (int j = 0; j < num_segments; j++)
			{
				Vec3 p0 = Vec3(spline_graph->calcSegmentPoint(i, float(j) / num_segments));
				Vec3 p1 = Vec3(spline_graph->calcSegmentPoint(i, float(j + 1) / num_segments));
				Visualizer::renderLine3D(p0, p1, vec4_white);
			}
		}
	}

	void move_box_along_graph()
	{
		float time = Game::getTime();

		float t = time - Math::floor(time / spline_graph->getNumSegments()) * spline_graph->getNumSegments();
		int segment_id = int(t);
		t -= float(segment_id);

		Vec3 p = Vec3(spline_graph->calcSegmentPoint(segment_id, t));
		vec3 direction = spline_graph->calcSegmentTangent(segment_id, t);
		vec3 up = spline_graph->calcSegmentUpVector(segment_id, t);

		box->setWorldPosition(p);
		box->setWorldDirection(direction, up);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	SplineGraphPtr spline_graph;
	ObjectMeshStaticPtr box;
};

REGISTER_COMPONENT(SplineGraphSample);
