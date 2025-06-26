#include <UnigineComponentSystem.h>
#include <UnigineVisualizer.h>
#include <UnigineWorlds.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

class WorldSplineGraphSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(WorldSplineGraphSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, spline_file);
	PROP_PARAM(File, segment_node);

private:
	void init()
	{
		Unigine::String spline_path = spline_file.get();
		if (spline_path.size() <= 0)
		{
			Unigine::Log::error("WorldSplineGraphSample::init(): Spline File param is empty!\n");
			return;
		}
		
		Unigine::String node_path = segment_node.get();
		if (node_path.size() <= 0)
		{
			Unigine::Log::error("WorldSplineGraphSample::init(): Segment File param is empty!\n");
			return;
		}
		
		world_spline_graph = WorldSplineGraph::create();

		// load the .spl spline file
		world_spline_graph->load(spline_path);

		Vector<SplineSegmentPtr> segments;
		world_spline_graph->getSplineSegments(segments);

		// for each spline segment in the graph assign to it a source node "road.node" and set its mode to "TILING"
		for (int i = 0; i < segments.size(); i += 1)
		{
			SplineSegmentPtr &segment = segments[i];
			segment->assignSource(node_path, SplineSegment::FORWARD_X);
			segment->setSegmentMode(node_path, SplineSegment::SEGMENT_TILING);
		}

		// rebuild the graph after changing the segments' parameters
		world_spline_graph->rebuild();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	WorldSplineGraphPtr world_spline_graph;
};

REGISTER_COMPONENT(WorldSplineGraphSample);
