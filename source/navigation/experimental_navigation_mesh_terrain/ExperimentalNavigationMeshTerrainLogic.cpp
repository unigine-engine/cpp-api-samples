// Interactive demo of hierarchical navigation on a large streamed terrain.
// The world holds two experimental navigation meshes: a fully resident low
// resolution mesh for the global plan and a dynamic high resolution mesh
// whose tiles are baked at runtime around the invokers. A group of animated
// characters walks to the point clicked on the terrain: each agent plans the
// global route on the low resolution mesh, splits it into waypoints, and
// refines every leg on the high resolution mesh streamed around it by its
// own invoker (see ExperimentalSeeker).
// Boxes and spheres with dynamic cut area volumes can be spawned and dragged
// with the manipulator, imitating a construction site: moving a volume
// automatically rebakes the touched tiles of both meshes, so the agents
// route around the buildings.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UnigineNodes.h>
#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../menu_ui/SampleWidgets.h"
#include "../../user_interface/widget_manipulators/Manipulators.h"
#include "../../utils/navigation/ExperimentalNavigationUtils.h"
#include "../../utils/navigation/ExperimentalSeeker.h"
#include "../../utils/navigation/ExperimentalSeekerCharacter.h"
#include "../../utils/navigation/ExperimentalSeekerCrowd.h"

class ExperimentalNavigationMeshTerrainLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshTerrainLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// the navigation mesh nodes; found in the world by the streaming mode
	// when not assigned
	PROP_PARAM(Node, navigation_high)
	PROP_PARAM(Node, navigation_low)
	// the assets of the agent and the building blocks; the sample defaults
	// are used when empty
	PROP_PARAM(File, agent_asset)
	PROP_PARAM(File, box_asset)
	PROP_PARAM(File, sphere_asset)
	PROP_PARAM(Int, num_agents, 8)
	// where the agents are (re)spawned; the spot in front of the camera is
	// used when empty
	PROP_PARAM(Node, spawn_point)
	// the intersection mask of the walkable ground: the click-to-move ray
	// looks for the target point on the surfaces matching it, and the spawned
	// nodes are dropped onto them
	PROP_PARAM(Mask, target_intersection_mask, 1)

private:
	void init();
	void update();
	void shutdown();

	// world interaction
	void handle_click();
	bool is_block(const Unigine::NodePtr &hit_node) const;
	bool ground_point(const Unigine::Math::Vec3 &around, Unigine::Math::Vec3 &ret_point) const;
	bool point_in_front(Unigine::Math::Vec3 &ret_point) const;

	// agents
	void spawn_agents();
	void clear_agents();
	void apply_agent_settings();

	// building blocks
	void spawn_block(const char *asset_path);
	void clear_blocks();
	void apply_carve_mode();

	// sample UI
	void create_ui();
	void update_status();

	Unigine::ExperimentalNavigationMeshPtr high_mesh;
	Unigine::ExperimentalNavigationMeshPtr low_mesh;
	ExperimentalSeekerCrowd *crowd{nullptr};
	Manipulators *manipulators{nullptr};

	Unigine::Vector<ExperimentalSeeker *> agents;
	Unigine::Vector<Unigine::NodePtr> agent_nodes;
	Unigine::Vector<Unigine::NodePtr> blocks;

	// runtime-adjustable settings
	int agents_count{8};
	float agent_speed{1.34f};
	bool show_debug{false};
	bool carve_while_dragging{true};
	bool show_local_navigation_mesh{false};
	bool show_global_navigation_mesh{false};

	Unigine::Math::Vec3 last_target{Unigine::Math::Vec3_zero};
	bool has_last_target{false};

	// the mouse travel of the current right button press, see handle_click()
	float click_mouse_travel{0.f};

	SampleDescriptionWindow sample_description_window;
	Unigine::EventConnections widget_connections;
};

REGISTER_COMPONENT(ExperimentalNavigationMeshTerrainLogic);

using namespace Unigine;
using namespace Unigine::Math;

// the sample assets used when the file parameters are left empty
static const char *DEFAULT_AGENT_ASSET = "cpp_component_samples/navigation/experimental_navigation_mesh_character/character.node";
static const char *DEFAULT_BOX_ASSET = "cpp_component_samples/navigation/experimental_navigation_mesh_terrain/box.node";
static const char *DEFAULT_SPHERE_ASSET = "cpp_component_samples/navigation/experimental_navigation_mesh_terrain/sphere.node";

// spacing of the agent spawn grid
constexpr float SPAWN_SPACING = 1.5f;

// the mouse travel (in pixels) above which a right button press counts as a
// camera look, not a click
constexpr float CLICK_DRAG_THRESHOLD = 8.f;

static const char *asset_or_default(const char *path, const char *default_path)
{
	return (path && *path) ? path : default_path;
}

void ExperimentalNavigationMeshTerrainLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// the meshes are either assigned or recognized by the streaming mode: the
	// high resolution mesh streams (dynamic bake), the low resolution one is
	// fully resident
	high_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation_high.get());
	low_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation_low.get());
	if (!high_mesh || !low_mesh)
	{
		Vector<NodePtr> nodes;
		World::getNodesByType(Node::EXPERIMENTAL_NAVIGATION_MESH, nodes);
		for (const NodePtr &mesh_node : nodes)
		{
			auto mesh = checked_ptr_cast<ExperimentalNavigationMesh>(mesh_node);
			if (!mesh)
				continue;
			bool streamed = mesh->isStreamingEnabled() || mesh->getBakeMode() == ExperimentalNavigationMesh::BAKE_MODE_DYNAMIC;
			if (streamed && !high_mesh)
				high_mesh = mesh;
			if (!streamed && !low_mesh)
				low_mesh = mesh;
		}
	}
	if (!high_mesh)
		Log::warning("ExperimentalNavigationMeshTerrainLogic: no streamed high resolution navigation mesh in the world\n");
	if (!low_mesh)
		Log::warning("ExperimentalNavigationMeshTerrainLogic: no resident low resolution navigation mesh in the world\n");

	// the crowd controller and the manipulators sit on the same logic node.
	// The crowd is optional: without it the agents drive themselves and do
	// not avoid each other
	crowd = getComponent<ExperimentalSeekerCrowd>(node);
	if (!crowd)
		Log::message("ExperimentalNavigationMeshTerrainLogic: no ExperimentalSeekerCrowd component on the logic node, the agents drive themselves\n");

	// the manipulator is configured in the world, the sample only picks it up
	manipulators = getComponent<Manipulators>(node);

	agents_count = num_agents;
	spawn_agents();

	create_ui();
}

void ExperimentalNavigationMeshTerrainLogic::update()
{
	if (!high_mesh || !low_mesh)
	{
		sample_description_window.setStatus("The world must contain two experimental navigation meshes: a streamed high resolution one and a resident low resolution one.");
		return;
	}

	// disable player controls while dragging a block: the manipulator hotkeys
	// share the keys with the camera
	if (manipulators)
		Game::getPlayer()->setControlled(!manipulators->isActive());

	handle_click();

	if (show_local_navigation_mesh)
		high_mesh->renderVisualizer();
	if (show_global_navigation_mesh)
		low_mesh->renderVisualizer();

	// the target is done once every agent has arrived: the marker goes out
	// and freshly respawned agents do not inherit the stale point
	if (has_last_target && agents.size() > 0)
	{
		bool all_reached = true;
		for (ExperimentalSeeker *agent : agents)
			all_reached = all_reached && agent->isTargetReached();
		if (all_reached)
			has_last_target = false;
	}

	// mark the commanded point
	if (has_last_target)
		Visualizer::renderCircle(0.75f, translate(last_target + Vec3(0.f, 0.f, 0.2f)), vec4(1.f, 0.85f, 0.f, 1.f), Game::getIFps());

	update_status();
}

void ExperimentalNavigationMeshTerrainLogic::shutdown()
{
	clear_agents();
	clear_blocks();
	widget_connections.disconnectAll();
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// A right click on the terrain sends all agents to the clicked point. The
// right button also drives the camera look, so only a short press without a
// mouse travel counts as a click; a click on a spawned block is ignored.
void ExperimentalNavigationMeshTerrainLogic::handle_click()
{
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_RIGHT))
		click_mouse_travel = 0.f;
	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON_RIGHT))
		click_mouse_travel += length(vec2(Input::getMouseDeltaPosition()));

	if (!Input::isMouseButtonUp(Input::MOUSE_BUTTON_RIGHT))
		return;
	if (click_mouse_travel > CLICK_DRAG_THRESHOLD)
		return;
	if (Gui::getCurrent()->getUnderCursorWidget())
		return;

	PlayerPtr player = Game::getPlayer();
	ivec2 mouse = Input::getMousePosition();
	Vec3 p0 = player->getWorldPosition();
	Vec3 p1 = p0 + Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y)) * Scalar(4000.0);

	WorldIntersectionPtr intersection = WorldIntersection::create();
	ObjectPtr hit = World::getIntersection(p0, p1, target_intersection_mask, intersection);
	if (!hit || is_block(hit))
		return;

	last_target = intersection->getPoint();
	has_last_target = true;

	// everyone walks to the same point: the arrival is loose enough
	// (target_reach_distance) for the agents not to fight for the exact spot
	for (ExperimentalSeeker *agent : agents)
		agent->setTargetPosition(last_target);
}

bool ExperimentalNavigationMeshTerrainLogic::is_block(const NodePtr &hit_node) const
{
	for (NodePtr current = hit_node; current; current = current->getParent())
	{
		if (blocks.contains(current))
			return true;
	}
	return false;
}

// Drops the point onto the ground with a vertical ray.
bool ExperimentalNavigationMeshTerrainLogic::ground_point(const Vec3 &around, Vec3 &ret_point) const
{
	WorldIntersectionPtr intersection = WorldIntersection::create();
	ObjectPtr hit = World::getIntersection(around + Vec3(0.f, 0.f, 200.f), around - Vec3(0.f, 0.f, 500.f), target_intersection_mask, intersection);
	if (!hit)
		return false;

	ret_point = intersection->getPoint();
	return true;
}

// The spot on the ground the camera looks at: a straight ray along the view
// direction down to the terrain.
bool ExperimentalNavigationMeshTerrainLogic::point_in_front(Vec3 &ret_point) const
{
	PlayerPtr player = Game::getPlayer();
	Vec3 p0 = player->getWorldPosition();
	Vec3 p1 = p0 + Vec3(player->getViewDirection()) * Scalar(4000.0);

	WorldIntersectionPtr intersection = WorldIntersection::create();
	ObjectPtr hit = World::getIntersection(p0, p1, target_intersection_mask, intersection);
	if (!hit)
		return false;

	ret_point = intersection->getPoint();
	return true;
}

// Spawns the agents on a grid in front of the camera. The agent asset carries
// an ExperimentalNavigationMeshInvoker, so the high resolution tiles are baked
// around each agent wherever it goes; the routes are planned hierarchically
// through the low resolution mesh.
void ExperimentalNavigationMeshTerrainLogic::spawn_agents()
{
	clear_agents();

	if (!high_mesh || !low_mesh)
		return;

	// the agents spawn around the spawn point when it is assigned, otherwise
	// in front of the camera
	Vec3 center;
	if (spawn_point)
	{
		center = spawn_point->getWorldPosition();
		ground_point(center, center);
	}
	else if (!point_in_front(center))
		return;

	const char *asset_path = asset_or_default(agent_asset.get(), DEFAULT_AGENT_ASSET);

	int side = (int)Math::ceil(Math::sqrt((float)agents_count));
	for (int i = 0; i < agents_count; i++)
	{
		Vec3 cell = center + Vec3((i % side - side / 2) * SPAWN_SPACING, (i / side - side / 2) * SPAWN_SPACING, 0.f);
		Vec3 position = cell;
		ground_point(cell, position);

		NodePtr agent_node = World::loadNode(asset_path);
		if (!agent_node)
		{
			Log::warning("ExperimentalNavigationMeshTerrainLogic: cannot load the agent asset \"%s\"\n", asset_path);
			return;
		}

		ExperimentalSeeker *seeker = getComponent<ExperimentalSeeker>(agent_node);
		if (!seeker)
			seeker = getComponentInChildren<ExperimentalSeeker>(agent_node);
		if (!seeker)
		{
			Log::warning("ExperimentalNavigationMeshTerrainLogic: the agent asset \"%s\" has no ExperimentalSeeker component\n", asset_path);
			agent_node.deleteLater();
			return;
		}

		agent_node->setWorldPosition(position);

		// the asset may carry the references of its authoring world: rewire
		// the seeker to the meshes of this one; the properties are assigned
		// before the component init runs
		seeker->navigation = high_mesh;
		seeker->global_navigation = low_mesh;
		seeker->movement_speed = agent_speed;
		seeker->debug_visualizer_enabled = show_debug;

		if (crowd)
			crowd->addAgent(seeker);

		agents.append(seeker);
		agent_nodes.append(agent_node);

		if (has_last_target)
			seeker->setTargetPosition(last_target);
	}
}

void ExperimentalNavigationMeshTerrainLogic::clear_agents()
{
	// unregister the agents before deleting their nodes
	if (crowd)
		crowd->clearAgents();

	for (NodePtr &agent_node : agent_nodes)
	{
		if (agent_node)
			agent_node.deleteLater();
	}
	agents.clear();
	agent_nodes.clear();
}

void ExperimentalNavigationMeshTerrainLogic::apply_agent_settings()
{
	for (ExperimentalSeeker *agent : agents)
	{
		agent->movement_speed = agent_speed;
		agent->debug_visualizer_enabled = show_debug;
	}
}

// Spawns a building block in front of the camera. The block asset carries a
// dynamic cut area volume: wherever the block is dragged, the volume cuts the
// walkable area out of both meshes, and the touched tiles are rebaked by the
// engine automatically.
void ExperimentalNavigationMeshTerrainLogic::spawn_block(const char *asset_path)
{
	Vec3 position;
	if (!point_in_front(position))
		return;

	NodePtr block = World::loadNode(asset_path);
	if (!block)
	{
		Log::warning("ExperimentalNavigationMeshTerrainLogic: cannot load the block asset \"%s\"\n", asset_path);
		return;
	}

	block->setWorldPosition(position);
	blocks.append(block);
}

void ExperimentalNavigationMeshTerrainLogic::clear_blocks()
{
	// deleting a block invalidates the area its volume covered
	for (NodePtr &block : blocks)
	{
		if (block)
			block.deleteLater();
	}
	blocks.clear();
}

// Applies the carve mode to the volumes of all blocks: while dragging, the
// walkable area follows the block continuously; in the stationary mode the
// rebake happens once the block is released and settles. Called only when
// the mode is switched in the UI: a freshly spawned block keeps the settings
// of its asset.
void ExperimentalNavigationMeshTerrainLogic::apply_carve_mode()
{
	for (const NodePtr &block : blocks)
	{
		if (!block)
			continue;

		Vector<NodePtr> hierarchy;
		hierarchy.append(block);
		block->getHierarchy(hierarchy);
		for (const NodePtr &child : hierarchy)
		{
			if (auto volume = checked_ptr_cast<ExperimentalNavigationMeshAreaVolume>(child))
				volume->setDynamicApplyOnlyWhenStationary(!carve_while_dragging);
		}
	}
}

void ExperimentalNavigationMeshTerrainLogic::update_status()
{
	String status;

	status += String::format("High resolution mesh: %d / %d tiles loaded, %d polygons\n",
		high_mesh->getNumLoadedTiles(), high_mesh->getNumTiles(), high_mesh->getNumPolygons());
	status += String::format("Low resolution mesh: %d polygons\n", low_mesh->getNumPolygons());
	status += String::format("Navigation tasks: %d%s%s\n",
		ExperimentalNavigation::getNumActiveTasks(),
		ExperimentalBakeNavigation::isBaking() ? ", baking..." : "",
		ExperimentalNavigation::isStreamingMemoryLimitReached() ? ", streaming memory limit reached!" : "");
	status += String::format("Agents: %d, blocks: %d\n", agents.size(), blocks.size());

	// the routes of the lead agent show how the hierarchical plan advances;
	// the failure reasons point at the misconfiguration when a route cannot
	// be built (e.g. no invoker streaming the tiles around the agent)
	if (agents.size() > 0)
	{
		ExperimentalSeeker *lead = agents[0];
		if (!lead->hasTarget())
			status += "Lead agent: no target, right-click the terrain to send the agents\n";
		else
		{
			String global_line = Utils::getRouteStatusName(lead->getGlobalRouteStatus());
			if (lead->getGlobalRouteStatus() != ExperimentalNavigationPath::STATUS_COMPLETE)
				global_line += String::format(" (%s)", Utils::getRouteFailureName(lead->getGlobalRouteFailure()));

			String local_line = Utils::getRouteStatusName(lead->getRouteStatus());
			if (lead->getRouteStatus() != ExperimentalNavigationPath::STATUS_COMPLETE)
				local_line += String::format(" (%s)", Utils::getRouteFailureName(lead->getRouteFailure()));

			status += String::format("Lead agent: global route %s, leg %d / %d, local route %s\n",
				global_line.get(),
				lead->getCurrentWaypointIndex() + 1, lead->getNumWaypoints(),
				local_line.get());
		}

		// the locomotion state of the lead character, e.g. to spot an agent
		// stuck in a turn or a stop
		if (agent_nodes.size() > 0)
		{
			auto *character = getComponentInChildren<ExperimentalSeekerCharacter>(agent_nodes[0]);
			if (character)
				status += String::format("Lead agent locomotion: %s / %s\n", character->getControlStateName(), character->getLocomotionStateName());
		}
	}

	sample_description_window.setStatus(status.get());
}

void ExperimentalNavigationMeshTerrainLogic::create_ui()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 600);
	auto window = sample_description_window.getWindow();

	// agents
	{
		auto group = WidgetGroupBox::create("Agents", 8, 8);
		window->addChild(group, Gui::ALIGN_LEFT);

		auto grid = WidgetGridBox::create(3);
		group->addChild(grid, Gui::ALIGN_EXPAND);

		add_int_parameter(widget_connections, grid, "Agents",
			"The number of agents, applied on respawn.",
			agents_count, 1, 50,
			[this](int value) {
				agents_count = value;
			});

		add_float_parameter(widget_connections, grid, "Movement speed",
			"The walking speed of the agents (in m/s): the locomotion animations are played at the matching rate.",
			agent_speed, 0.7f, 3.f,
			[this](float value) {
				agent_speed = value;
				apply_agent_settings();
			});

		add_button(widget_connections, group, "Respawn agents",
			"Respawns the agents on a grid at the spawn point (or in front of the camera).",
			[this]() { spawn_agents(); });
	}

	// construction
	{
		auto group = WidgetGroupBox::create("Construction", 8, 8);
		window->addChild(group, Gui::ALIGN_LEFT);

		// the block actions share one row
		auto buttons = WidgetHBox::create(4, 4);
		group->addChild(buttons, Gui::ALIGN_EXPAND);

		add_button(widget_connections, buttons, "Spawn box",
			"Spawns a box in front of the camera. Drag it with the manipulator: the cut volume of the box carves the walkable area out of both meshes.",
			[this]() { spawn_block(asset_or_default(box_asset.get(), DEFAULT_BOX_ASSET)); });

		add_button(widget_connections, buttons, "Spawn sphere",
			"Spawns a sphere in front of the camera. Drag it with the manipulator: the cut volume of the sphere carves the walkable area out of both meshes.",
			[this]() { spawn_block(asset_or_default(sphere_asset.get(), DEFAULT_SPHERE_ASSET)); });

		add_button(widget_connections, buttons, "Clear blocks",
			"Deletes all spawned blocks; the areas they covered are rebaked.",
			[this]() { clear_blocks(); });

		add_bool_parameter(widget_connections, group, "Carve while dragging",
			"When enabled, the walkable area follows the dragged block continuously; otherwise the touched tiles are rebaked once the block is released. Switching the mode overrides the volume settings of all spawned blocks.",
			carve_while_dragging,
			[this](bool enabled) {
				carve_while_dragging = enabled;
				apply_carve_mode();
			});
	}

	// visualization
	{
		auto group = WidgetGroupBox::create("Visualization", 8, 8);
		window->addChild(group, Gui::ALIGN_LEFT);

		add_bool_parameter(widget_connections, group, "Show Seeker Debug",
			"Draws the local route corridor and the waypoints of the global route of every agent.",
			show_debug,
			[this](bool enabled) {
				show_debug = enabled;
				apply_agent_settings();
			});

		add_bool_parameter(widget_connections, group, "Show Local Navigation Mesh",
			"Draws the streamed high resolution mesh: the loaded tiles follow the agents.",
			show_local_navigation_mesh,
			[this](bool enabled) {
				show_local_navigation_mesh = enabled;
			});

		add_bool_parameter(widget_connections, group, "Show Global Navigation Mesh",
			"Draws the fully resident low resolution mesh the global routes are planned on.",
			show_global_navigation_mesh,
			[this](bool enabled) {
				show_global_navigation_mesh = enabled;
			});
	}

	window->arrange();
}
