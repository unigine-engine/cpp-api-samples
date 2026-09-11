// Demonstrates pathfinding on an experimental navigation mesh using
// ExperimentalNavigationPathFetch. Start and end points can be moved via
// manipulators, pathfinding settings can be tweaked in the sample window.
// A complete path is drawn with the route color, a partial path is drawn up
// to the last reachable point with the remaining segment shown as a red line.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/navigation/ExperimentalNavigationUtils.h"

class ExperimentalNavigationMeshLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, path_start)
	PROP_PARAM(Node, path_end)
	PROP_PARAM(Node, navigation)
	PROP_PARAM(Color, route_color, Unigine::Math::vec4_white)

private:
	void init();
	void update();
	void shutdown();

	// pathfinding, self-contained and reusable
	void init_navigation();
	void apply_settings();
	void update_route();

	// sample UI
	void create_ui();
	void update_status();

	Unigine::ExperimentalNavigationMeshPtr navigation_mesh;
	Unigine::ExperimentalNavigationMeshFilterPtr filter;
	Unigine::ExperimentalNavigationPathFetchPtr route_fetch;
	Unigine::ExperimentalNavigationPathPtr route;

	// runtime-adjustable settings, pushed to the filter and the path fetch
	// by apply_settings(); initial values are taken from the engine defaults
	// and the bake settings of the navigation mesh in init_navigation()
	Unigine::ExperimentalNavigationPath::STRAIGHT_MODE straight_mode;
	float heuristic_scale;
	float max_cost;
	int max_search_nodes;
	float agent_radius;
	float agent_height;
	float max_slope_angle;
	float max_step_height;
	float snap_size;
	bool consider_obstacles;

	// sample visualization
	bool show_navigation_mesh{true};

	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator;
};


REGISTER_COMPONENT(ExperimentalNavigationMeshLogic);

using namespace Unigine;
using namespace Unigine::Math;

void ExperimentalNavigationMeshLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	init_navigation();

	// manipulators move the start and end points, scaling is disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setXAxisScale(false);
		widget_manipulator->setYAxisScale(false);
		widget_manipulator->setZAxisScale(false);
	}

	create_ui();
}

void ExperimentalNavigationMeshLogic::update()
{
	// disable player controls while dragging manipulators
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	if (show_navigation_mesh && navigation_mesh)
		navigation_mesh->renderVisualizer();

	update_route();
	update_status();
}

void ExperimentalNavigationMeshLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Creates the navigation mesh filter and the path fetch restricted to the
// navigation mesh assigned to the component.
void ExperimentalNavigationMeshLogic::init_navigation()
{
	navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation.get());

	filter = ExperimentalNavigationMeshFilter::create();
	route_fetch = ExperimentalNavigationPathFetch::create();
	route_fetch->setNavigationMesh(navigation_mesh);

	// start with the parameters the navigation mesh was baked with,
	// so the mesh matches the filter by default
	if (navigation_mesh)
	{
		filter->setNavigationMask(navigation_mesh->getNavigationMask());

		auto bake_settings = navigation_mesh->getBakeSettings();
		filter->setAgentRadius(bake_settings->getAgentRadius());
		filter->setAgentHeight(bake_settings->getAgentHeight());
		filter->setMaxSlopeAngle(bake_settings->getMaxSlopeAngle());
		filter->setMaxStepHeight(bake_settings->getMaxStepHeight());
	}

	straight_mode = ExperimentalNavigationPath::STRAIGHT_MODE_ALL_CROSSINGS;
	heuristic_scale = route_fetch->getHeuristicScale();
	max_cost = route_fetch->getMaxCost();
	max_search_nodes = route_fetch->getMaxSearchNodes();
	agent_radius = filter->getAgentRadius();
	agent_height = filter->getAgentHeight();
	max_slope_angle = filter->getMaxSlopeAngle();
	max_step_height = filter->getMaxStepHeight();
	snap_size = filter->getSnapSize().z;
	consider_obstacles = filter->getObstacleMask() != 0;

	apply_settings();
}

// Pushes the current settings to the filter and the path fetch.
void ExperimentalNavigationMeshLogic::apply_settings()
{
	filter->setAgentRadius(agent_radius);
	filter->setAgentHeight(agent_height);
	filter->setMaxSlopeAngle(max_slope_angle);
	filter->setMaxStepHeight(max_step_height);
	filter->setSnapSize(vec3(snap_size));
	filter->setObstacleMask(consider_obstacles ? ~0 : 0);

	route_fetch->setFilter(filter);
	route_fetch->setStraightMode(straight_mode);
	route_fetch->setHeuristicScale(heuristic_scale);
	route_fetch->setMaxCost(max_cost);
	route_fetch->setMaxSearchNodes(max_search_nodes);
}

// Computes the route between the start and end points and visualizes it.
// A partial route is drawn up to the last reachable point with the
// unreachable remainder shown as a red line.
void ExperimentalNavigationMeshLogic::update_route()
{
	if (!path_start || !path_end)
		return;

	auto start = path_start->getWorldPosition();
	auto end = path_end->getWorldPosition();

	route_fetch->fetchForce(start, end);
	route = route_fetch->takePath();

	auto status = route ? route->getStatus() : ExperimentalNavigationPath::STATUS_NO_PATH;
	if (status == ExperimentalNavigationPath::STATUS_COMPLETE)
	{
		route->renderVisualizer(route_color);
	}
	else if (status == ExperimentalNavigationPath::STATUS_PARTIAL && route->getNumPoints() > 0)
	{
		route->renderVisualizer(route_color);
		Visualizer::renderLine3D(route->getPoint(route->getNumPoints() - 1), end, vec4_red, Game::getIFps());
	}
	else
		Visualizer::renderLine3D(start, end, vec4_red, Game::getIFps());
}

void ExperimentalNavigationMeshLogic::create_ui()
{
	sample_description_window.createWindow();

	sample_description_window.addSwitchParameter(
		"Straight mode",
		"Defines which points are included in the resulting path: only corners, corners with area crossings, or all polygon crossings.",
		straight_mode,
		{"Corners", "Area crossings", "All crossings"},
		[this](int mode) {
			straight_mode = (ExperimentalNavigationPath::STRAIGHT_MODE)mode;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Heuristic scale",
		"The weight of the A* heuristic. Values close to 1 give the optimal path, higher values speed up the search at the cost of path optimality.",
		heuristic_scale,
		0.f,
		5.f,
		[this](float value) {
			heuristic_scale = value;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Max cost",
		"The maximum allowed cost of the route. When the limit is reached, the path is built partially up to the last reachable point.",
		min(max_cost, 500.f),
		0.f,
		500.f,
		[this](float value) {
			max_cost = value;
			apply_settings();
		}
	);

	sample_description_window.addIntParameter(
		"Max search nodes",
		"The maximum number of nodes visited during the path search. Low values may cause the search to stop before the target is reached.",
		min(max_search_nodes, 16384),
		0,
		16384,
		[this](int value) {
			max_search_nodes = value;
			apply_settings();
		}
	);

	sample_description_window.addParameterSpacer();

	sample_description_window.addFloatParameter(
		"Agent radius",
		"The agent radius used to select a suitable navigation mesh and keep the route away from boundaries.",
		agent_radius,
		0.f,
		5.f,
		[this](float value) {
			agent_radius = value;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Agent height",
		"The agent height used to select a suitable navigation mesh.",
		agent_height,
		0.f,
		10.f,
		[this](float value) {
			agent_height = value;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Max slope angle",
		"The maximum slope angle (in degrees) the agent can walk on.",
		max_slope_angle,
		0.f,
		90.f,
		[this](float value) {
			max_slope_angle = value;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Max step height",
		"The maximum height of an obstacle the agent can step over.",
		max_step_height,
		0.f,
		2.f,
		[this](float value) {
			max_step_height = value;
			apply_settings();
		}
	);

	sample_description_window.addFloatParameter(
		"Snap size",
		"The search extents used to snap the start and end points onto the navigation mesh.",
		snap_size,
		0.f,
		10.f,
		[this](float value) {
			snap_size = value;
			apply_settings();
		}
	);

	sample_description_window.addBoolParameter(
		"Consider obstacles",
		"Toggles whether obstacles block the route.",
		consider_obstacles,
		[this](bool enabled) {
			consider_obstacles = enabled;
			apply_settings();
		}
	);

	sample_description_window.addParameterSpacer();

	sample_description_window.addBoolParameter(
		"Show Navigation Mesh",
		"Draws the navigation mesh colored by the areas of its polygons.",
		show_navigation_mesh,
		[this](bool enabled) {
			show_navigation_mesh = enabled;
		}
	);
}

void ExperimentalNavigationMeshLogic::update_status()
{
	String status = "Route status: no path\n";
	if (route)
	{
		status = String::format(
			"Route status: %s\n"
			"Route length: %.2f\n"
			"Route cost: %.2f\n"
			"Route points: %d\n",
			Utils::getRouteStatusName(route->getStatus()),
			route->getLength(),
			route->getCost(),
			route->getNumPoints()
		);

		if (route->getFailureReason() != ExperimentalNavigationPath::FAILURE_NONE)
			status += String::format("Failure reason: %s\n", Utils::getRouteFailureName(route->getFailureReason()));
	}

	sample_description_window.setStatus(status);
}
