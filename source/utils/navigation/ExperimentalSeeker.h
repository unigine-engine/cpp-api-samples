#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineExperimentalNavigation.h>
#include <UnigineWorld.h>
// PROP_SWITCH_ENUM
#include "../Utils.h"

PROP_SWITCH_ENUM(STRAIGHT_MODE, Corners, AreaCrossings, AllCrossings);

// Walks its node to the target along a route on the navigation mesh, unless
// an ExperimentalSeekerCrowd drives the movement. A PathfindingTarget on the
// target node is optional; so is the coarse mesh for the streamed worlds.
class ExperimentalSeeker : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ExperimentalSeeker, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	// after the movement, including the root motion of the subclasses
	COMPONENT_POST_UPDATE(render_debug_visualizer, 1);

	PROP_TOOLTIP("The node to walk to; a moving target is followed");
	PROP_PARAM(Node, target, nullptr);
	PROP_TOOLTIP("The navigation mesh the routes are planned on");
	PROP_PARAM(Node, navigation, nullptr);

	PROP_GROUP("Global Navigation");
	PROP_TOOLTIP("Coarse fully loaded mesh for the streamed worlds: the global route is planned on it and walked by waypoints on the main mesh");
	PROP_PARAM(Node, global_navigation, nullptr);
	PROP_TOOLTIP("Vertical range of snapping the route ends to the coarse mesh: covers the height difference between the meshes");
	PROP_PARAM(Float, global_snap_size, 2.0f);
	PROP_TOOLTIP("Distance between the waypoints of the global route; keep it inside the streaming range of the main mesh");
	PROP_PARAM(Float, waypoint_spacing, 30.0f);
	PROP_TOOLTIP("Distance at which a waypoint counts as passed");
	PROP_PARAM(Float, waypoint_reach_distance, 3.0f);

	PROP_GROUP("Movement");
	PROP_TOOLTIP("Walk speed, m/s");
	PROP_PARAM(Float, movement_speed, 2.0f);
	PROP_TOOLTIP("Turn rate toward the movement direction, deg/s; the agent slows down while misaligned");
	PROP_PARAM(Float, rotation_speed, 360.0f);
	PROP_TOOLTIP("Velocity change per second the avoidance solver may command; low values make standing agents start slowly");
	PROP_PARAM(Float, max_acceleration, 20.0f);
	PROP_TOOLTIP("Distance at which the target counts as reached and the agent stops");
	PROP_PARAM(Float, target_reach_distance, 0.5f);
	PROP_TOOLTIP("Put the agent on the geometry below: the navigation mesh only approximates the floor height");
	PROP_PARAM(Toggle, snap_to_ground, true);
	PROP_TOOLTIP("Intersection mask of the ground geometry");
	PROP_PARAM(Mask, ground_intersection_mask, 1);

	PROP_GROUP("Fetch Settings");
	PROP_TOOLTIP("Points of the straightened route: the corners only, plus the area borders, or every polygon edge");
	PROP_PARAM(Switch, straight_mode, Corners, STRAIGHT_MODE_STR);
	PROP_TOOLTIP("A* heuristic weight: 1 - the shortest route, higher - faster search, rougher routes");
	PROP_PARAM(Float, heuristic_scale, 1.0f);
	PROP_TOOLTIP("Search budget by the route cost, 0 - unlimited; longer routes come out partial");
	PROP_PARAM(Float, max_cost, 200.0f);
	PROP_TOOLTIP("Search budget by the visited polygons, 0 - unlimited; bounds the cost of unreachable targets");
	PROP_PARAM(Int, max_search_nodes, 1000);

	PROP_GROUP("Agent Settings");
	PROP_TOOLTIP("Body radius: selects a suitable navigation mesh and sizes the agent in the avoidance solver");
	PROP_PARAM(Float, agent_radius, 0.3f);
	PROP_TOOLTIP("Body height: selects a suitable navigation mesh and sizes the agent in the avoidance solver");
	PROP_PARAM(Float, agent_height, 2.0f);
	PROP_TOOLTIP("Steepest slope the agent accepts of an automatically selected mesh, 0 - any");
	PROP_PARAM(Float, max_slope_angle, 45.0f);
	PROP_TOOLTIP("Highest step the agent accepts of an automatically selected mesh, 0 - any");
	PROP_PARAM(Float, max_step_height, 0.4f);
	PROP_TOOLTIP("Size of the box that snaps the route ends onto the navigation mesh; unrelated to snap_to_ground");
	PROP_PARAM(Float, snap_size, 0.1f);
	PROP_TOOLTIP("Mask of the obstacles respected in the route search and in the corridor, 0 - ignore all obstacles");
	PROP_PARAM(Mask, obstacle_mask, ~0);
	PROP_TOOLTIP("Own body obstacle of the agent, excluded from its route search so the agent never blocks itself");
	PROP_PARAM(Node, body_obstacle, nullptr);

	PROP_GROUP("Replan Settings");
	PROP_TOOLTIP("Cooldown of retrying a failed or partial route");
	PROP_PARAM(Float, replan_retry_time, 0.2f);
	PROP_TOOLTIP("Cooldown of the global route rebuilds; also rate limits chasing a moving target");
	PROP_PARAM(Float, global_replan_retry_time, 0.4f);
	PROP_TOOLTIP("Time in place while trying to move before the route is rebuilt, 0 - disabled");
	PROP_PARAM(Float, stall_replan_time, 1.0f);

	PROP_GROUP("Debug Visualizer");
	PROP_TOOLTIP("Render the routes and the velocities of the agent");
	PROP_PARAM(Toggle, debug_visualizer_enabled, false);
	PROP_PARAM(Toggle, render_path, true, nullptr, "Render the corridor and the unreachable remainder of the route", nullptr, "debug_visualizer_enabled=1");
	PROP_PARAM(Color, path_color, Unigine::Math::vec4_white, nullptr, "Corridor color", nullptr, "debug_visualizer_enabled=1;render_path=1");
	PROP_PARAM(Toggle, render_global_path, true, nullptr, "Render the waypoints of the global route", nullptr, "debug_visualizer_enabled=1");
	PROP_PARAM(Color, global_path_color, Unigine::Math::vec4(1.0f, 1.0f, 1.0f, 0.5f), nullptr, "Waypoint color", nullptr, "debug_visualizer_enabled=1;render_global_path=1");
	PROP_PARAM(Toggle, render_velocities, true, nullptr, "Render the desired, commanded and actual velocities", nullptr, "debug_visualizer_enabled=1");
	PROP_PARAM(Color, desired_velocity_color, Unigine::Math::vec4_green, nullptr, "The route intent", nullptr, "debug_visualizer_enabled=1;render_velocities=1");
	PROP_PARAM(Color, command_velocity_color, Unigine::Math::vec4_red, nullptr, "The command after the avoidance", nullptr, "debug_visualizer_enabled=1;render_velocities=1");
	PROP_PARAM(Color, velocity_color, Unigine::Math::vec4_blue, nullptr, "The velocity measured from the movement", nullptr, "debug_visualizer_enabled=1;render_velocities=1");

	// applies the cached Fetch/Agent settings and forces a replan; the other parameters are read every frame
	void applySettings();

	void setTargetNode(const Unigine::NodePtr &target_node);
	void setTargetPosition(const Unigine::Math::Vec3 &position);
	void clearTarget();
	bool hasTarget() const;
	Unigine::Math::Vec3 getTargetPosition() const;
	bool isTargetReached() const { return target_reached; }

	// fills desired_velocity with the route intent; called by ExperimentalSeekerCrowd before the solve
	virtual void computeControl();
	// executes a velocity command, own or solver-adjusted; an override must keep command_velocity and the measured velocity updated
	virtual void moveWithVelocity(const Unigine::Math::vec3 &new_velocity, float ifps);
	void correctPosition(const Unigine::Math::Vec3 &position);

	void teleport(const Unigine::Math::Vec3 &position);

	// the own body obstacle, e.g. for the per agent solver exclusions
	const Unigine::ObstaclePtr &getBodyObstacle() const { return own_obstacle; }

	// per agent avoidance parameters, applied by ExperimentalSeekerCrowd
	void setAvoidancePriority(float priority) { avoidance_priority = priority; }
	float getAvoidancePriority() const { return avoidance_priority; }
	void setAvoidanceInteractionMask(int mask) { avoidance_interaction_mask = mask; }
	int getAvoidanceInteractionMask() const { return avoidance_interaction_mask; }

	void setControlledExternally(bool controlled) { controlled_externally = controlled; }
	bool isControlledExternally() const { return controlled_externally; }

	const Unigine::Math::vec3 &getDesiredVelocity() const { return desired_velocity; }
	const Unigine::Math::vec3 &getVelocity() const { return velocity; }
	Unigine::ExperimentalNavigationPath::STATUS getRouteStatus() const { return route_status; }
	Unigine::ExperimentalNavigationPath::FAILURE getRouteFailure() const { return route_failure; }

	bool isHierarchical() const { return (bool)global_navigation_mesh; }
	Unigine::ExperimentalNavigationPath::STATUS getGlobalRouteStatus() const { return global_status; }
	Unigine::ExperimentalNavigationPath::FAILURE getGlobalRouteFailure() const { return global_failure; }
	int getNumWaypoints() const { return waypoints.size(); }
	int getCurrentWaypointIndex() const { return waypoint_index; }

protected:
	void init();
	bool resolve_target_position(Unigine::Math::Vec3 &ret_position) const;

	// places the node at the given navigation mesh position, snapped to the
	// ground geometry, see snap_to_ground
	void place_node(const Unigine::Math::Vec3 &position);

	void render_debug_visualizer();

	Unigine::ExperimentalNavigationMeshPtr navigation_mesh;
	Unigine::ExperimentalNavigationMeshFilterPtr filter;
	Unigine::ExperimentalNavigationPathFetchPtr route_fetch;
	Unigine::ExperimentalNavigationMeshCorridorPtr corridor;
	Unigine::ObstaclePtr own_obstacle;

	Unigine::Math::vec3 desired_velocity{Unigine::Math::vec3_zero};
	Unigine::Math::vec3 velocity{Unigine::Math::vec3_zero};
	Unigine::Math::vec3 command_velocity{Unigine::Math::vec3_zero};

	// height of the velocity vectors origin, tuned by the subclasses
	float debug_visualizer_height{1.0f};

	bool target_reached{false};

private:
	void update();

	void update_route(float ifps);
	bool update_stall(float ifps);
	void update_global_route(const Unigine::Math::Vec3 &target_position);
	void replan_global_route(const Unigine::Math::Vec3 &target_position);
	void update_waypoint_shortcut();
	bool current_waypoint(Unigine::Math::Vec3 &ret_waypoint);
	void replan_route(const Unigine::Math::Vec3 &local_target);

	Unigine::ExperimentalNavigationPath::STATUS route_status{Unigine::ExperimentalNavigationPath::STATUS_NO_PATH};
	Unigine::ExperimentalNavigationPath::FAILURE route_failure{Unigine::ExperimentalNavigationPath::FAILURE_NONE};
	Unigine::Math::Vec3 planned_local_target;
	float replan_timer{0.0f};

	// the stall watchdog: the anchor position and the time spent near it
	Unigine::Math::Vec3 stall_position{Unigine::Math::Vec3_zero};
	float stall_timer{0.0f};

	// the ground snap ray: the reused hit and the own nodes it must skip
	Unigine::WorldIntersectionPtr ground_hit;
	Unigine::Vector<Unigine::NodePtr> ground_exclude;

	// target source: the node target is used unless a fixed position is set
	bool use_target_node{true};
	bool has_fixed_target{false};
	Unigine::Math::Vec3 fixed_target_position{Unigine::Math::Vec3_zero};

	// the global plan: the goal it was built for and the waypoints to walk
	Unigine::ExperimentalNavigationMeshPtr global_navigation_mesh;
	Unigine::ExperimentalNavigationMeshFilterPtr global_filter;
	Unigine::ExperimentalNavigationPathFetchPtr global_fetch;
	Unigine::ExperimentalNavigationPath::STATUS global_status{Unigine::ExperimentalNavigationPath::STATUS_NO_PATH};
	Unigine::ExperimentalNavigationPath::FAILURE global_failure{Unigine::ExperimentalNavigationPath::FAILURE_NONE};
	Unigine::Vector<Unigine::Math::Vec3> waypoints;
	int waypoint_index{0};
	Unigine::Math::Vec3 planned_target_position;
	float global_replan_timer{0.0f};
	// cooldown of the shortcut test fetches, see update_waypoint_shortcut
	float shortcut_timer{0.0f};

	bool controlled_externally{false};

	float avoidance_priority{-1.0f};
	int avoidance_interaction_mask{~0};
};
