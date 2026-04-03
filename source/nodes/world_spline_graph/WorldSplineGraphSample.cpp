// Demonstrates WorldSplineGraph for placing geometry along spline paths in the world.
// Unlike SplineGraph (data-only), WorldSplineGraph instantiates source nodes along
// each segment. TILING mode repeats the source geometry, while STRETCH mode scales it.

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

	// Path to the .spl spline file defining the curve
	PROP_PARAM(File, spline_file);
	// Node file to instantiate along the spline (e.g., road segment)
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

		// Create WorldSplineGraph node that will generate geometry
		world_spline_graph = WorldSplineGraph::create();

		// Load spline data from .spl file
		world_spline_graph->load(spline_path);

		// Get all segments in the loaded spline
		Vector<SplineSegmentPtr> segments;
		world_spline_graph->getSplineSegments(segments);

		// Configure each segment to use the source node
		for (int i = 0; i < segments.size(); i += 1)
		{
			SplineSegmentPtr &segment = segments[i];
			// Assign source node and specify forward axis (X in this case)
			segment->assignSource(node_path, SplineSegment::FORWARD_X);
			// TILING mode repeats the geometry to fill segment length
			// STRETCH mode would scale a single instance to fit
			segment->setSegmentMode(node_path, SplineSegment::SEGMENT_TILING);
		}

		// Regenerate all geometry after configuration changes
		// This instantiates the source nodes along each segment
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
	// WorldSplineGraph node that generates geometry along the path
	WorldSplineGraphPtr world_spline_graph;
};

REGISTER_COMPONENT(WorldSplineGraphSample);
