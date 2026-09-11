// Interactive crowd demo on an experimental navigation mesh. A configurable
// number of agents is spawned at runtime and driven by ExperimentalSeekerCrowd
// through the ExperimentalNavigationAvoidance solver. Scenarios: two teams
// crossing through the bottleneck gates, agents swapping places across a
// circle, and random wandering. The sample exposes the solver settings, a VIP
// agent priority, the team interaction masks, the agent density heatmap, and
// a teleport button demonstrating the separation of overlapping agents.

#include <UnigineVisualizer.h>
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UnigineNodes.h>
#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../menu_ui/SampleWidgets.h"
#include "../../utils/navigation/ExperimentalSeeker.h"
#include "../../utils/navigation/ExperimentalSeekerCrowd.h"

class ExperimentalNavigationMeshCrowdLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalNavigationMeshCrowdLogic, Unigine::ComponentBase)
	COMPONENT_INIT(init, 2)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, navigation)
	PROP_PARAM(File, agent_red)
	PROP_PARAM(File, agent_blue)
	PROP_PARAM(Node, spawn_a)
	PROP_PARAM(Node, spawn_b)
	PROP_PARAM(Int, num_agents, 100)

private:
	enum SCENARIO
	{
		SCENARIO_BOTTLENECK = 0,
		SCENARIO_CIRCLE,
		SCENARIO_WANDER,
	};

	enum TEAM
	{
		TEAM_RED = 0,
		TEAM_BLUE,
	};

	struct Agent
	{
		ExperimentalSeeker *seeker{nullptr};
		Unigine::NodePtr node;
		Unigine::NodePtr target;
		int team{TEAM_RED};
		// own random avoidance priority, rolled at spawn
		float avoidance_priority{0.f};
		// the two ends the agent walks between in the bottleneck and circle
		// scenarios
		Unigine::Math::Vec3 point_a{Unigine::Math::Vec3_zero};
		Unigine::Math::Vec3 point_b{Unigine::Math::Vec3_zero};
		bool to_b{true};
	};

	void init();
	void update();
	void shutdown();

	// scenario control
	void restart_scenario();
	void clear_agents();
	bool is_spawn_cell_valid(const Unigine::Math::Vec3 &base, const Unigine::Math::Vec3 &cell, Unigine::Math::Vec3 &ret_snapped);
	Unigine::Vector<Unigine::Math::Vec3> build_spawn_grid(const Unigine::Math::Vec3 &base, int count);
	bool spawn_agent(int team, const Unigine::Math::Vec3 &position, const Unigine::Math::Vec3 &target_position);
	void retarget_agent(Agent &agent);
	Unigine::Math::Vec3 random_reachable_point(const Unigine::Math::Vec3 &around, float max_cost);
	void teleport_all();

	// per-agent solver overrides
	void apply_team_masks();
	void apply_obstacle_masks();
	void apply_vip();

	// sample UI
	void create_ui();
	void render_density();
	void update_status();

	Unigine::ExperimentalNavigationMeshPtr navigation_mesh;
	Unigine::ExperimentalNavigationMeshFilterPtr filter;
	Unigine::ExperimentalNavigationPathFetchPtr reach_fetch;
	ExperimentalSeekerCrowd *crowd{nullptr};

	Unigine::Vector<Agent> agents;

	// runtime-adjustable settings
	int scenario{SCENARIO_WANDER};
	int agents_count{50};
	float circle_radius{10.f};
	bool vip_enabled{false};
	bool teams_ignore_each_other{false};
	bool use_obstacles{false};
	bool density_enabled{false};
	float density_cell_size{1.f};
	bool show_debug{false};
	bool show_navigation_mesh{false};

	int goals_reached{0};

	SampleDescriptionWindow sample_description_window;
	Unigine::EventConnections widget_connections;
};


REGISTER_COMPONENT(ExperimentalNavigationMeshCrowdLogic);

using namespace Unigine;
using namespace Unigine::Math;

// distance to the target at which the goal counts as reached
static constexpr float REACH_DISTANCE = 1.0f;

// spacing of the spawn grid cells
static constexpr float SPAWN_SPACING = 1.2f;

// mask bits of the teams: the avoidance interaction masks when the teams
// ignore each other, and the obstacle masks of the team body obstacles
// preconfigured on the obstacle nodes of the agent assets in the editor
static constexpr int TEAM_MASKS[] = {1, 2};

// avoidance priority of the VIP agent, well above the random 0..5 priorities
// of the crowd
static constexpr float VIP_PRIORITY = 20.f;

void ExperimentalNavigationMeshCrowdLogic::init()
{
	Visualizer::setEnabled(true);
	Console::run("experimental_navigation_show_mesh 0");
	Console::run("experimental_navigation_show_mesh_mode 1");
	Console::run("experimental_navigation_show_mesh_depth_test 1");
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation.get());

	// the filter starts with the parameters the navigation mesh was baked
	// with, so the mesh matches the filter by default
	filter = ExperimentalNavigationMeshFilter::create();
	if (navigation_mesh)
	{
		filter->setNavigationMask(navigation_mesh->getNavigationMask());

		auto bake_settings = navigation_mesh->getBakeSettings();
		filter->setAgentRadius(bake_settings->getAgentRadius());
		filter->setAgentHeight(bake_settings->getAgentHeight());
	}

	// used to validate the spawn grid cells by reachability
	reach_fetch = ExperimentalNavigationPathFetch::create();
	reach_fetch->setFilter(filter);
	reach_fetch->setNavigationMesh(navigation_mesh);

	// the crowd controller component must be added in the world,
	// the sample only picks it up
	crowd = getComponent<ExperimentalSeekerCrowd>(node);
	if (!crowd)
	{
		Unigine::Vector<ExperimentalSeekerCrowd *> crowds;
		getComponentsInWorld<ExperimentalSeekerCrowd>(crowds);
		if (crowds.size() > 0)
			crowd = crowds[0];
	}
	if (!crowd)
		Log::warning("ExperimentalNavigationMeshCrowdLogic: no ExperimentalSeekerCrowd component in the world\n");

	// the body obstacles are respected by the route planning only: the solver
	// already represents the agents natively, their obstacles would double them
	if (crowd && crowd->getAvoidance())
		crowd->getAvoidance()->setObstacleMask(0);

	agents_count = num_agents;
	restart_scenario();

	create_ui();
}

void ExperimentalNavigationMeshCrowdLogic::update()
{
	if (!navigation_mesh || !crowd)
	{
		sample_description_window.setStatus("Assign the experimental navigation mesh node and add the ExperimentalSeekerCrowd component to the world.");
		return;
	}

	// retarget the agents that reached their goals; the positions are taken
	// from the seeker node: the component may sit on a child of the asset
	for (auto &agent : agents)
	{
		if (!agent.seeker || !agent.target)
			continue;

		Vec3 delta = agent.target->getWorldPosition() - agent.seeker->getNode()->getWorldPosition();
		delta.z = 0.0;
		if (delta.length2() < REACH_DISTANCE * REACH_DISTANCE)
		{
			goals_reached += 1;
			retarget_agent(agent);
		}
	}

	// highlight the VIP agent
	if (vip_enabled && agents.size() > 0 && agents[0].seeker)
		Visualizer::renderCircle(0.7f, translate(agents[0].seeker->getNode()->getWorldPosition() + Vec3(0.f, 0.f, 0.1f)), vec4(1.f, 1.f, 0.f, 1.f), Game::getIFps());

	if (show_navigation_mesh)
		navigation_mesh->renderVisualizer();

	render_density();
	update_status();
}

void ExperimentalNavigationMeshCrowdLogic::shutdown()
{
	clear_agents();
	widget_connections.disconnectAll();
	Visualizer::setEnabled(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	sample_description_window.shutdown();
}

// Respawns the agents according to the current scenario.
void ExperimentalNavigationMeshCrowdLogic::restart_scenario()
{
	clear_agents();
	goals_reached = 0;

	if (!navigation_mesh || !crowd)
		return;

	Vec3 base_a = spawn_a ? spawn_a->getWorldPosition() : navigation_mesh->getWorldPosition();
	Vec3 base_b = spawn_b ? spawn_b->getWorldPosition() : navigation_mesh->getWorldPosition();

	int count = max(agents_count, 2);

	switch (scenario)
	{
		// two teams facing each other across the gates. The spawn grids are
		// fit into the walkable area around the bases, the slots of the two
		// grids are paired: each agent walks back and forth between its own
		// slot and the paired slot on the opposite side
		case SCENARIO_BOTTLENECK:
		{
			Vector<Vec3> grid_a = build_spawn_grid(base_a, count / 2);
			Vector<Vec3> grid_b = build_spawn_grid(base_b, count / 2);
			int pairs = min(grid_a.size(), grid_b.size());

			for (int i = 0; i < pairs; i += 1)
			{
				if (spawn_agent(TEAM_RED, grid_a[i], grid_b[i]))
				{
					Agent &agent = agents.last();
					agent.point_a = agent.seeker->getNode()->getWorldPosition();
					agent.point_b = grid_b[i];
				}
				if (spawn_agent(TEAM_BLUE, grid_b[i], grid_a[i]))
				{
					Agent &agent = agents.last();
					agent.point_a = agent.seeker->getNode()->getWorldPosition();
					agent.point_b = grid_a[i];
				}
			}
			break;
		}

		// agents evenly placed on a circle walk to the diametrically opposite
		// points, meeting each other in the center; the circle is centered on
		// the navigation mesh (the middle of the arena)
		case SCENARIO_CIRCLE:
		{
			Vec3 center = navigation_mesh->getWorldPosition();
			if (spawn_a)
				center.z = spawn_a->getWorldPosition().z;

			for (int i = 0; i < count; i += 1)
			{
				float angle = Consts::PI2 * i / count;
				Vec3 offset = Vec3(Math::cos(angle), Math::sin(angle), 0.f) * circle_radius;

				// both ends of the diameter must be on the mesh and reachable
				Vec3 start_point, end_point;
				if (!is_spawn_cell_valid(center, center + offset, start_point)
					|| !is_spawn_cell_valid(center, center - offset, end_point))
					continue;

				if (!spawn_agent(i % 2, start_point, end_point))
					continue;

				Agent &agent = agents.last();
				agent.point_a = agent.seeker->getNode()->getWorldPosition();
				agent.point_b = end_point;
			}
			break;
		}

		// agents wander between random reachable points
		case SCENARIO_WANDER:
		{
			for (int i = 0; i < count; i += 1)
			{
				Vec3 position;
				if (!navigation_mesh->findRandomPoint(filter, position))
					position = base_a;
				spawn_agent(i % 2, position, random_reachable_point(position, 30.f));
			}
			break;
		}
	}

	apply_team_masks();
	apply_obstacle_masks();
	apply_vip();
}

void ExperimentalNavigationMeshCrowdLogic::clear_agents()
{
	// unregister the agents before deleting their nodes
	if (crowd)
		crowd->clearAgents();

	for (auto &agent : agents)
	{
		if (agent.node)
			agent.node.deleteLater();
		if (agent.target)
			agent.target.deleteLater();
	}
	agents.clear();
}

// Checks that a spawn cell lies on the mesh close to the requested position
// and is directly reachable from the base: a complete path not much longer
// than the straight line rejects the cells behind the walls and the gates.
bool ExperimentalNavigationMeshCrowdLogic::is_spawn_cell_valid(const Vec3 &base, const Vec3 &cell, Vec3 &ret_snapped)
{
	if (!navigation_mesh->findNearestPoint(filter, cell, ret_snapped))
		return false;

	Vec3 delta = ret_snapped - cell;
	delta.z = 0.0;
	if (delta.length2() > (0.6f * SPAWN_SPACING) * (0.6f * SPAWN_SPACING))
		return false;

	if (!reach_fetch->fetchForce(base, ret_snapped))
		return false;
	auto path = reach_fetch->takePath();
	if (!path || path->getStatus() != ExperimentalNavigationPath::STATUS_COMPLETE)
		return false;

	float straight = (float)length(ret_snapped - base);
	return path->getLength() <= straight * 1.5f + 3.f;
}

// Builds a spawn grid fit into the walkable area: the cells grow ring by
// ring around the base and only the valid ones are kept, so the grid never
// leaks off the mesh or to the other side of the walls.
Vector<Vec3> ExperimentalNavigationMeshCrowdLogic::build_spawn_grid(const Vec3 &base, int count)
{
	constexpr int max_ring = 24;

	Vector<Vec3> cells;
	Vec3 snapped;

	if (is_spawn_cell_valid(base, base, snapped))
		cells.append(snapped);

	for (int ring = 1; ring <= max_ring && cells.size() < count; ring += 1)
	{
		for (int y = -ring; y <= ring && cells.size() < count; y += 1)
		{
			for (int x = -ring; x <= ring && cells.size() < count; x += 1)
			{
				// only the cells of the current ring
				if (max(abs(x), abs(y)) != ring)
					continue;

				Vec3 cell = base + Vec3(x * SPAWN_SPACING, y * SPAWN_SPACING, 0.f);
				if (is_spawn_cell_valid(base, cell, snapped))
					cells.append(snapped);
			}
		}
	}

	return cells;
}

// Spawns a single agent of the team at the position and registers it in the
// crowd. The seeker properties are assigned before the component init runs.
bool ExperimentalNavigationMeshCrowdLogic::spawn_agent(int team, const Vec3 &position, const Vec3 &target_position)
{
	const char *asset_path = team == TEAM_RED ? agent_red.get() : agent_blue.get();
	NodePtr agent_node = World::loadNode(asset_path);
	if (!agent_node)
	{
		Log::warning("ExperimentalNavigationMeshCrowdLogic: cannot load the agent asset \"%s\"\n", asset_path);
		return false;
	}

	ExperimentalSeeker *seeker = getComponent<ExperimentalSeeker>(agent_node);
	if (!seeker)
		seeker = getComponentInChildren<ExperimentalSeeker>(agent_node);
	if (!seeker)
	{
		Log::warning("ExperimentalNavigationMeshCrowdLogic: the agent asset \"%s\" has no ExperimentalSeeker component\n", asset_path);
		agent_node.deleteLater();
		return false;
	}

	// an agent must never spawn off the mesh: its route fetch would fail and
	// it would stand still forever; fall back to a random reachable point
	Vec3 snapped = position;
	if (!navigation_mesh->findNearestPoint(filter, position, snapped))
		snapped = random_reachable_point(position, 10.f);
	agent_node->setWorldPosition(snapped);

	NodePtr target_node = NodeDummy::create();
	target_node->setWorldPosition(target_position);

	seeker->navigation = navigation.get();
	seeker->target = target_node;
	seeker->debug_visualizer_enabled = show_debug;

	// unequal agents break the symmetric head-on deadlocks (e.g. in the middle
	// of the circle scenario): each agent gets a random avoidance priority and
	// a randomized walk speed
	seeker->movement_speed = seeker->movement_speed * Game::getRandomFloat(0.8f, 1.2f);

	crowd->addAgent(seeker);

	Agent agent;
	agent.seeker = seeker;
	agent.node = agent_node;
	agent.target = target_node;
	agent.team = team;
	agent.avoidance_priority = Game::getRandomFloat(0.f, 5.f);
	agents.append(agent);
	return true;
}

// Assigns the next goal to an agent that reached its target.
void ExperimentalNavigationMeshCrowdLogic::retarget_agent(Agent &agent)
{
	switch (scenario)
	{
		case SCENARIO_BOTTLENECK:
		case SCENARIO_CIRCLE:
			agent.to_b = !agent.to_b;
			agent.target->setWorldPosition(agent.to_b ? agent.point_b : agent.point_a);
			break;

		case SCENARIO_WANDER:
		{
			// retry a few times so the next point is not right next to the agent
			Vec3 position = agent.seeker->getNode()->getWorldPosition();
			Vec3 next = position;
			for (int attempt = 0; attempt < 8; attempt += 1)
			{
				next = random_reachable_point(position, 30.f);
				if ((next - position).length2() > 25.0)
					break;
			}
			agent.target->setWorldPosition(next);
			break;
		}
	}
}

Vec3 ExperimentalNavigationMeshCrowdLogic::random_reachable_point(const Vec3 &around, float max_cost)
{
	Vec3 point;
	if (navigation_mesh->findRandomReachablePoint(filter, around, max_cost, point))
		return point;
	if (navigation_mesh->findRandomPoint(filter, point))
		return point;
	return around;
}

// Teleports all agents into a small disk, demonstrating how the crowd
// separates overlapping agents.
void ExperimentalNavigationMeshCrowdLogic::teleport_all()
{
	Vec3 center = spawn_a ? spawn_a->getWorldPosition() : navigation_mesh->getWorldPosition();
	for (auto &agent : agents)
	{
		if (!agent.seeker)
			continue;
		vec3 offset = vec3(Game::getRandomFloat(-1.5f, 1.5f), Game::getRandomFloat(-1.5f, 1.5f), 0.f);
		agent.seeker->teleport(center + Vec3(offset));
	}
}

// When the teams ignore each other, each team gets its own interaction mask
// bit, so the cross-team mask overlap is empty.
void ExperimentalNavigationMeshCrowdLogic::apply_team_masks()
{
	for (auto &agent : agents)
	{
		if (!agent.seeker)
			continue;
		agent.seeker->setAvoidanceInteractionMask(teams_ignore_each_other ? TEAM_MASKS[agent.team] : ~0);
	}
}

// Assigns the obstacle mask each agent respects: none while the body obstacles
// are disabled, the obstacles of both teams by default, only the own team ones
// when the teams ignore each other. The own obstacle of an agent is excluded
// by the seeker itself.
void ExperimentalNavigationMeshCrowdLogic::apply_obstacle_masks()
{
	for (auto &agent : agents)
	{
		if (!agent.seeker)
			continue;

		int mask = 0;
		if (use_obstacles)
			mask = teams_ignore_each_other ? TEAM_MASKS[agent.team] : (TEAM_MASKS[TEAM_RED] | TEAM_MASKS[TEAM_BLUE]);

		agent.seeker->obstacle_mask = mask;
		agent.seeker->applySettings();
	}
}

// The VIP priority is applied to the first agent, others keep the random
// priorities rolled at spawn.
void ExperimentalNavigationMeshCrowdLogic::apply_vip()
{
	for (auto &agent : agents)
	{
		if (agent.seeker)
			agent.seeker->setAvoidancePriority(agent.avoidance_priority);
	}

	if (vip_enabled && agents.size() > 0 && agents[0].seeker)
		agents[0].seeker->setAvoidancePriority(VIP_PRIORITY);
}

// Draws the agent density heatmap over the navigation mesh bake area.
void ExperimentalNavigationMeshCrowdLogic::render_density()
{
	if (!density_enabled)
		return;

	const auto &avoidance = crowd->getAvoidance();
	if (!avoidance)
		return;

	vec3 bake_size = navigation_mesh->getBakeSettings()->getBakeSize();
	Vec3 center = navigation_mesh->getWorldPosition();
	Scalar ground_z = spawn_a ? spawn_a->getWorldPosition().z : center.z;

	float cell = max(density_cell_size, 0.5f);
	int half_x = min((int)(bake_size.x * 0.5f / cell), 40);
	int half_y = min((int)(bake_size.y * 0.5f / cell), 40);

	for (int y = -half_y; y <= half_y; y += 1)
	{
		for (int x = -half_x; x <= half_x; x += 1)
		{
			Vec3 point = Vec3(center.x + x * cell, center.y + y * cell, ground_z);
			float density = avoidance->getAgentDensity(point);
			if (density < 0.05f)
				continue;

			vec4 color = lerp(vec4(0.f, 1.f, 0.f, 0.35f), vec4(1.f, 0.f, 0.f, 0.5f), saturate(density * 0.25f));
			Visualizer::renderSolidBox(vec3(cell * 0.95f, cell * 0.95f, 0.02f), translate(point + Vec3(0.f, 0.f, 0.05f)), color, Game::getIFps());
		}
	}
}

void ExperimentalNavigationMeshCrowdLogic::update_status()
{
	static const char *scenario_names[] = {"Bottleneck", "Circle", "Wander"};

	// agents without a complete route signal a broken spawn or an
	// unreachable target
	int no_route = 0;
	for (const auto &agent : agents)
	{
		if (agent.seeker && agent.seeker->getRouteStatus() != ExperimentalNavigationPath::STATUS_COMPLETE)
			no_route += 1;
	}

	sample_description_window.setStatus(String::format(
		"Scenario: %s\n"
		"Agents: %d\n"
		"Agents without a route: %d\n"
		"Goals reached: %d\n",
		scenario_names[scenario],
		agents.size(),
		no_route,
		goals_reached));
}

void ExperimentalNavigationMeshCrowdLogic::create_ui()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 600);
	auto window = sample_description_window.getWindow();

	// all the parameter groups live in one scroll box to keep the window compact
	auto scroll_box = WidgetScrollBox::create();
	scroll_box->setHeight(420);
	window->addChild(scroll_box, Gui::ALIGN_EXPAND);

	auto scroll_content = WidgetVBox::create(0, 4);
	scroll_box->addChild(scroll_content, Gui::ALIGN_EXPAND);

	// scenario control
	{
		auto group = WidgetGroupBox::create("Scenario", 8, 8);
		scroll_content->addChild(group, Gui::ALIGN_EXPAND);

		auto grid = WidgetGridBox::create(3, 5, 5);
		group->addChild(grid, Gui::ALIGN_EXPAND);

		add_switch_parameter(widget_connections, grid, "Scenario",
			"Bottleneck: two teams cross through the gates. Circle: agents swap places across a circle. Wander: agents walk between random points.",
			scenario, {"Bottleneck", "Circle", "Wander"},
			[this](int value) {
				scenario = value;
				restart_scenario();
			});

		add_int_parameter(widget_connections, grid, "Agents",
			"The number of agents, applied on restart.",
			agents_count, 10, 300,
			[this](int value) {
				agents_count = value;
			});

		add_float_parameter(widget_connections, grid, "Circle radius",
			"The radius of the circle scenario, applied on restart.",
			circle_radius, 2.f, 20.f,
			[this](float value) {
				circle_radius = value;
			});

		add_button(widget_connections, group, "Restart scenario",
			"Respawns the agents with the current settings.",
			[this]() { restart_scenario(); });

		add_button(widget_connections, group, "Teleport all to one point",
			"Teleports all agents into a small disk: the crowd pushes the overlapping agents apart.",
			[this]() { teleport_all(); });
	}

	// avoidance solver settings, pushed to the reusable crowd component
	if (crowd)
	{
		auto group = WidgetGroupBox::create("Avoidance solver", 8, 8);
		scroll_content->addChild(group, Gui::ALIGN_EXPAND);

		auto grid = WidgetGridBox::create(3, 5, 5);
		group->addChild(grid, Gui::ALIGN_EXPAND);

		add_float_parameter(widget_connections, grid, "Neighbor range",
			"The range within which the agents react to each other. The solver recommends at least 2 * (max_speed * prediction_time + agent_radius).",
			crowd->neighbor_range, 1.f, 20.f,
			[this](float value) { crowd->neighbor_range = value; crowd->applySettings(); });

		add_int_parameter(widget_connections, grid, "Max neighbors",
			"The maximum number of neighbors an agent takes into account.",
			crowd->max_neighbors, 1, 32,
			[this](int value) { crowd->max_neighbors = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Prediction time",
			"How far ahead (in seconds) the agents predict the collisions.",
			crowd->prediction_time, 0.f, 2.f,
			[this](float value) { crowd->prediction_time = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Desired velocity",
			"The weight of following the desired velocity.",
			crowd->weight_desired_velocity, 0.f, 5.f,
			[this](float value) { crowd->weight_desired_velocity = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Current velocity",
			"The weight of keeping the current velocity.",
			crowd->weight_current_velocity, 0.f, 5.f,
			[this](float value) { crowd->weight_current_velocity = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Side preference",
			"The weight of preferring one side when avoiding.",
			crowd->weight_side, 0.f, 5.f,
			[this](float value) { crowd->weight_side = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Time to impact",
			"The weight of avoiding the imminent collisions.",
			crowd->weight_time_to_impact, 0.f, 5.f,
			[this](float value) { crowd->weight_time_to_impact = value; crowd->applySettings(); });

		add_float_parameter(widget_connections, grid, "Separation",
			"The weight of keeping distance from the neighbors.",
			crowd->weight_separation, 0.f, 5.f,
			[this](float value) { crowd->weight_separation = value; crowd->applySettings(); });
	}

	// per-agent solver overrides
	{
		auto group = WidgetGroupBox::create("Agents", 8, 8);
		scroll_content->addChild(group, Gui::ALIGN_EXPAND);

		add_bool_parameter(widget_connections, group, "VIP agent",
			"Gives the first agent a fixed high avoidance priority (the regular agents get a random one from 0 to 5 at spawn): the crowd yields to it. The VIP agent is highlighted with a circle.",
			vip_enabled,
			[this](bool enabled) {
				vip_enabled = enabled;
				apply_vip();
			});

		add_bool_parameter(widget_connections, group, "Teams ignore each other",
			"Gives each team its own interaction mask, so the agents avoid only their teammates and respect only their body obstacles.",
			teams_ignore_each_other,
			[this](bool enabled) {
				teams_ignore_each_other = enabled;
				apply_team_masks();
				apply_obstacle_masks();
			});

		add_bool_parameter(widget_connections, group, "Use obstacles",
			"Makes the agents respect the body obstacles of the others: a standing group obstructs the walkable area and the routes are rebuilt around it. The own obstacle of an agent never blocks it.",
			use_obstacles,
			[this](bool enabled) {
				use_obstacles = enabled;
				apply_obstacle_masks();
			});
	}

	// visualization
	{
		auto group = WidgetGroupBox::create("Visualization", 8, 8);
		scroll_content->addChild(group, Gui::ALIGN_EXPAND);

		auto grid = WidgetGridBox::create(3, 5, 5);
		group->addChild(grid, Gui::ALIGN_EXPAND);

		add_bool_parameter(widget_connections, grid, "Show Seeker Debug",
			"Draws the route corridor and the velocities of every agent.",
			show_debug,
			[this](bool enabled) {
				show_debug = enabled;
				for (auto &agent : agents)
				{
					if (agent.seeker)
						agent.seeker->debug_visualizer_enabled = enabled;
				}
			});

		add_bool_parameter(widget_connections, grid, "Show Navigation Mesh",
			"Draws the navigation mesh colored by the areas of its polygons.",
			show_navigation_mesh,
			[this](bool enabled) {
				show_navigation_mesh = enabled;
			});

		add_bool_parameter(widget_connections, grid, "Density heatmap",
			"Visualizes the agent density computed by the avoidance solver.",
			density_enabled,
			[this](bool enabled) {
				density_enabled = enabled;
				if (crowd && crowd->getAvoidance())
				{
					crowd->getAvoidance()->setDensityEnabled(enabled);
					crowd->getAvoidance()->setDensityCellSize(density_cell_size);
				}
			});

		add_float_parameter(widget_connections, grid, "Density cell",
			"The cell size of the density grid.",
			density_cell_size, 0.5f, 4.f,
			[this](float value) {
				density_cell_size = value;
				if (crowd && crowd->getAvoidance())
					crowd->getAvoidance()->setDensityCellSize(value);
			});
	}

	window->arrange();
}
