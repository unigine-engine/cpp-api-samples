// Interactive demo with multiple ExperimentalSeeker agents chasing a target
// on an experimental navigation mesh. Each seeker computes a route with
// ExperimentalNavigationPathFetch and follows it using
// ExperimentalNavigationMeshCorridor. When a seeker reaches the target, the
// target relocates to a random position from a predefined set. Every seeker
// can be configured individually on its own tab of the sample window.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../menu_ui/SampleWidgets.h"
#include "../../utils/navigation/ExperimentalSeeker.h"
#include "../../utils/navigation/ExperimentalSeekerCrowd.h"

class ExperimentalNavigationMeshDemoLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshDemoLogic, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Color, path_color, Unigine::Math::vec4_white);
	PROP_PARAM(Color, selected_path_color, Unigine::Math::vec4(1.0f, 1.0f, 0.0f, 1.0f));

private:
	void init();
	void update();
	void shutdown();

	void create_ui();
	void create_seekers_ui();
	void create_seeker_tab(const Unigine::WidgetGridBoxPtr &grid, ExperimentalSeeker *seeker);
	void highlight_selected_seeker();

	// sample visualization
	bool show_navigation_mesh{true};

	Unigine::Vector<Unigine::ExperimentalNavigationMeshPtr> navigation_meshes;
	SampleDescriptionWindow sample_description_window;
	Manipulators *widget_manipulator;
	Unigine::EventConnections widget_connections;

	Unigine::WidgetTabBoxPtr seekers_tabbox;
	Unigine::Vector<ExperimentalSeeker *> tab_seekers;
};

REGISTER_COMPONENT(ExperimentalNavigationMeshDemoLogic);

using namespace Unigine;
using namespace Unigine::Math;

void ExperimentalNavigationMeshDemoLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// manipulators move the target and obstacles, scaling is disabled
	widget_manipulator = getComponent<Manipulators>(node);
	if (widget_manipulator)
	{
		widget_manipulator->setXAxisScale(false);
		widget_manipulator->setYAxisScale(false);
		widget_manipulator->setZAxisScale(false);
	}

	// every seeker draws its debug visualizer by default; the crowd controller
	// does not collect agents itself: register all the seekers in it if one is
	// present in the world
	Unigine::Vector<ExperimentalSeeker *> world_seekers;
	getComponentsInWorld<ExperimentalSeeker>(world_seekers);

	Unigine::Vector<ExperimentalSeekerCrowd *> crowds;
	getComponentsInWorld<ExperimentalSeekerCrowd>(crowds);

	for (auto *seeker : world_seekers)
	{
		seeker->debug_visualizer_enabled = true;
		if (crowds.size() > 0)
			crowds[0]->addAgent(seeker);
	}

	// the navigation meshes of the world, for the visualization toggle
	Unigine::Vector<NodePtr> mesh_nodes;
	World::getNodesByType(Node::EXPERIMENTAL_NAVIGATION_MESH, mesh_nodes);
	for (const NodePtr &mesh_node : mesh_nodes)
	{
		if (auto mesh = checked_ptr_cast<ExperimentalNavigationMesh>(mesh_node))
			navigation_meshes.append(mesh);
	}

	create_ui();
}

void ExperimentalNavigationMeshDemoLogic::update()
{
	// disable player controls while dragging manipulators
	Game::getPlayer()->setControlled(!widget_manipulator->isActive());

	if (show_navigation_mesh)
	{
		for (const auto &mesh : navigation_meshes)
			mesh->renderVisualizer();
	}
}

void ExperimentalNavigationMeshDemoLogic::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

void ExperimentalNavigationMeshDemoLogic::create_ui()
{
	sample_description_window.createWindow();

	create_seekers_ui();

	// visualization shared by all seekers
	auto group = WidgetGroupBox::create("Visualization", 8, 8);
	sample_description_window.getWindow()->addChild(group, Gui::ALIGN_LEFT);

	add_bool_parameter(widget_connections, group,
		"Show Navigation Mesh",
		"Draws the navigation mesh colored by the areas of its polygons.",
		show_navigation_mesh,
		[this](bool enabled) {
			show_navigation_mesh = enabled;
		});
}

// Creates a tab box with a tab per seeker holding its individual settings.
void ExperimentalNavigationMeshDemoLogic::create_seekers_ui()
{
	Unigine::Vector<ExperimentalSeeker *> world_seekers;
	getComponentsInWorld<ExperimentalSeeker>(world_seekers);
	if (world_seekers.empty())
		return;

	auto group = WidgetGroupBox::create("Seekers", 8, 8);
	sample_description_window.getWindow()->addChild(group, Gui::ALIGN_LEFT);

	seekers_tabbox = WidgetTabBox::create(4, 4);
	group->addChild(seekers_tabbox, Gui::ALIGN_EXPAND);

	for (int i = 0; i < world_seekers.size(); i += 1)
	{
		auto *seeker = world_seekers[i];

		String tab_name = seeker->getNode()->getName();
		if (tab_name.empty())
			tab_name = String::format("Seeker %d", i);

		seekers_tabbox->setCurrentTab(seekers_tabbox->addTab(tab_name));
		tab_seekers.append(seeker);

		auto grid = WidgetGridBox::create(3, 5, 5);
		seekers_tabbox->addChild(grid, Gui::ALIGN_EXPAND);
		create_seeker_tab(grid, seeker);
	}

	seekers_tabbox->getEventChanged().connect(widget_connections, [this]() {
		highlight_selected_seeker();
	});

	if (seekers_tabbox->getNumTabs() > 0)
		seekers_tabbox->setCurrentTab(0);
	highlight_selected_seeker();
}

void ExperimentalNavigationMeshDemoLogic::create_seeker_tab(const WidgetGridBoxPtr &grid, ExperimentalSeeker *seeker)
{
	add_float_parameter(widget_connections, grid,
		"Movement speed",
		"The movement speed of the agent.",
		seeker->movement_speed,
		0.f,
		20.f,
		[seeker](float value) {
			seeker->movement_speed = value;
		}
	);

	add_switch_parameter(widget_connections, grid,
		"Straight mode",
		"Defines which points are included in the resulting path: only corners, corners with area crossings, or all polygon crossings.",
		seeker->straight_mode.get(),
		{"Corners", "Area crossings", "All crossings"},
		[seeker](int mode) {
			seeker->straight_mode = (ExperimentalNavigationPath::STRAIGHT_MODE)mode;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Heuristic scale",
		"The weight of the A* heuristic. Values close to 1 give the optimal path, higher values speed up the search at the cost of path optimality.",
		seeker->heuristic_scale.get(),
		0.f,
		5.f,
		[seeker](float value) {
			seeker->heuristic_scale = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Max cost",
		"The maximum allowed cost of the route. When the limit is reached, the path is built partially up to the last reachable point.",
		min(seeker->max_cost.get(), 500.f),
		0.f,
		500.f,
		[seeker](float value) {
			seeker->max_cost = value;
			seeker->applySettings();
		}
	);

	add_int_parameter(widget_connections, grid,
		"Max search nodes",
		"The maximum number of nodes visited during the path search. Low values may cause the search to stop before the target is reached.",
		min(seeker->max_search_nodes.get(), 16384),
		0,
		16384,
		[seeker](int value) {
			seeker->max_search_nodes = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Agent radius",
		"The agent radius used to select a suitable navigation mesh and keep the route away from boundaries.",
		seeker->agent_radius.get(),
		0.f,
		1.f,
		[seeker](float value) {
			seeker->agent_radius = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Agent height",
		"The agent height used to select a suitable navigation mesh.",
		seeker->agent_height.get(),
		0.f,
		4.f,
		[seeker](float value) {
			seeker->agent_height = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Max slope angle",
		"The maximum slope angle (in degrees) the agent can walk on.",
		seeker->max_slope_angle.get(),
		0.f,
		90.f,
		[seeker](float value) {
			seeker->max_slope_angle = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Max step height",
		"The maximum height of an obstacle the agent can step over.",
		seeker->max_step_height.get(),
		0.f,
		2.f,
		[seeker](float value) {
			seeker->max_step_height = value;
			seeker->applySettings();
		}
	);

	add_float_parameter(widget_connections, grid,
		"Snap size",
		"The search extents used to snap the route points onto the navigation mesh.",
		seeker->snap_size.get(),
		0.f,
		10.f,
		[seeker](float value) {
			seeker->snap_size = value;
			seeker->applySettings();
		}
	);

	add_bool_parameter(widget_connections, grid,
		"Consider obstacles",
		"Toggles whether obstacles block the route.",
		seeker->obstacle_mask.get() != 0,
		[seeker](bool enabled) {
			seeker->obstacle_mask = enabled ? ~0 : 0;
			seeker->applySettings();
		}
	);

	add_bool_parameter(widget_connections, grid,
		"Show Seeker Debug",
		"Draws the route corridor and the velocities of this agent.",
		seeker->debug_visualizer_enabled.get() != 0,
		[seeker](bool enabled) {
			seeker->debug_visualizer_enabled = enabled;
		}
	);
}

// The route of the seeker selected in the tab box is drawn in green.
void ExperimentalNavigationMeshDemoLogic::highlight_selected_seeker()
{
	int current = seekers_tabbox ? seekers_tabbox->getCurrentTab() : -1;
	for (int i = 0; i < tab_seekers.size(); i += 1)
	{
		if (i == current)
			tab_seekers[i]->path_color = selected_path_color.get();
		else
			tab_seekers[i]->path_color = path_color.get();
	}
}
