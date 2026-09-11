#include "ExperimentalSeeker.h"

#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "Target.h"

REGISTER_COMPONENT(ExperimentalSeeker);

using namespace Unigine;
using namespace Math;

void ExperimentalSeeker::init()
{
	navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(navigation.get());

	filter = ExperimentalNavigationMeshFilter::create();
	if (navigation_mesh)
		filter->setNavigationMask(navigation_mesh->getNavigationMask());

	// the own body obstacle must never block the agent itself
	own_obstacle = checked_ptr_cast<Obstacle>(body_obstacle.get());
	if (own_obstacle)
		filter->addExcludeObstacle(own_obstacle);

	route_fetch = ExperimentalNavigationPathFetch::create();
	route_fetch->setFilter(filter);
	route_fetch->setNavigationMesh(navigation_mesh);

	corridor = ExperimentalNavigationMeshCorridor::create();

	// the ground snap ray must not hit the agent itself
	ground_hit = WorldIntersection::create();
	ground_exclude.append(node);
	node->getHierarchy(ground_exclude);

	// the global plan runs on the low resolution mesh with its own filter
	global_navigation_mesh = checked_ptr_cast<ExperimentalNavigationMesh>(global_navigation.get());
	if (global_navigation_mesh)
	{
		global_filter = ExperimentalNavigationMeshFilter::create();
		global_filter->setNavigationMask(global_navigation_mesh->getNavigationMask());
		// obstacles are handled by the local route
		global_filter->setObstacleMask(0);

		global_fetch = ExperimentalNavigationPathFetch::create();
		global_fetch->setFilter(global_filter);
		global_fetch->setNavigationMesh(global_navigation_mesh);
	}

	applySettings();
}

void ExperimentalSeeker::update()
{
	float ifps = Game::getIFps();
	update_route(ifps);

	// an external controller (e.g. ExperimentalSeekerCrowd) drives the agent
	if (controlled_externally)
		return;

	computeControl();
	moveWithVelocity(desired_velocity, ifps);
}

// The post-update of the component: runs at the order 1, after the movement,
// including the root motion of the subclasses.
void ExperimentalSeeker::render_debug_visualizer()
{
	if (!debug_visualizer_enabled || !corridor)
		return;

	float ifps = Game::getIFps();

	// nothing to highlight without a target or once it is reached
	bool has_route_to_show = hasTarget() && !target_reached;

	if (render_path && has_route_to_show)
	{
		if (route_status == ExperimentalNavigationPath::STATUS_COMPLETE)
			corridor->renderVisualizer(path_color);
		else if (route_status == ExperimentalNavigationPath::STATUS_PARTIAL && corridor->getNumNextCorners() > 0)
		{
			corridor->renderVisualizer(path_color);
			Visualizer::renderLine3D(corridor->getNextCorner(corridor->getNumNextCorners() - 1), planned_local_target, vec4_red, ifps);
		}
		else
			Visualizer::renderLine3D(node->getWorldPosition(), planned_local_target, vec4_red, ifps);
	}

	if (render_global_path && has_route_to_show)
	{
		Vec3 lift = Vec3(0.0, 0.0, 1.0);
		for (int i = waypoint_index; i < waypoints.size(); i++)
		{
			Visualizer::renderCircle(0.5f, translate(waypoints[i] + lift), global_path_color, ifps);
			if (i + 1 < waypoints.size())
				Visualizer::renderLine3D(waypoints[i] + lift, waypoints[i + 1] + lift, global_path_color, ifps);
		}
	}

	if (render_velocities)
	{
		// stacked so the matching vectors do not hide each other
		Vec3 step = Vec3(0.0, 0.0, 0.1);
		Vec3 origin = node->getWorldPosition() + Vec3(0.0f, 0.0f, debug_visualizer_height);
		Visualizer::renderVector(origin, origin + Vec3(desired_velocity), desired_velocity_color, 0.25f, false, ifps);
		Visualizer::renderVector(origin + step, origin + step + Vec3(command_velocity), command_velocity_color, 0.25f, false, ifps);
		Visualizer::renderVector(origin + step * 2.0f, origin + step * 2.0f + Vec3(velocity), velocity_color, 0.25f, false, ifps);
	}
}

void ExperimentalSeeker::applySettings()
{
	// may be called right after spawn, before init: init applies everything itself
	if (!filter || !corridor)
		return;

	// the fetches reference the filters, updating them in place is enough
	filter->setAgentRadius(agent_radius.get());
	filter->setAgentHeight(agent_height.get());
	filter->setMaxSlopeAngle(max_slope_angle.get());
	filter->setMaxStepHeight(max_step_height.get());
	filter->setSnapSize(vec3(snap_size.get()));
	filter->setObstacleMask(obstacle_mask.get());

	// the corridor checks the obstacles on its own, keep the mask in sync
	corridor->setObstacleMask(obstacle_mask.get());

	// the global filter shares the agent profile, see init for the obstacles
	if (global_filter)
	{
		global_filter->setAgentRadius(agent_radius.get());
		global_filter->setAgentHeight(agent_height.get());
		global_filter->setMaxSlopeAngle(max_slope_angle.get());
		global_filter->setMaxStepHeight(max_step_height.get());
		global_filter->setSnapSize(vec3(0.0f, 0.0f, global_snap_size.get()));
	}

	route_fetch->setStraightMode(ExperimentalNavigationPath::STRAIGHT_MODE(straight_mode.get()));
	route_fetch->setHeuristicScale(heuristic_scale.get());
	route_fetch->setMaxCost(max_cost.get());
	route_fetch->setMaxSearchNodes(max_search_nodes.get());

	// force a replan with the updated settings
	waypoints.clear();
	replan_timer = 0.0f;
	global_replan_timer = 0.0f;
	shortcut_timer = 0.0f;
	route_status = ExperimentalNavigationPath::STATUS_NO_PATH;
	stall_position = node->getWorldPosition();
	stall_timer = 0.0f;
}

void ExperimentalSeeker::setTargetNode(const NodePtr &target_node)
{
	target = target_node;
	use_target_node = true;
	has_fixed_target = false;
	target_reached = false;
}

void ExperimentalSeeker::setTargetPosition(const Vec3 &position)
{
	use_target_node = false;
	has_fixed_target = true;
	fixed_target_position = position;
	target_reached = false;
}

void ExperimentalSeeker::clearTarget()
{
	use_target_node = false;
	has_fixed_target = false;
	target_reached = true;
}

bool ExperimentalSeeker::hasTarget() const
{
	return ((use_target_node && target) || has_fixed_target);
}

Vec3 ExperimentalSeeker::getTargetPosition() const
{
	Vec3 position = Vec3_zero;
	resolve_target_position(position);
	return position;
}

void ExperimentalSeeker::computeControl()
{
	desired_velocity = vec3_zero;

	if (!corridor || !corridor->isValid())
		return;

	Vec3 target_position;
	if (!resolve_target_position(target_position))
		return;

	// the distance is horizontal: the target may hover above the ground
	Vec3 to_target = target_position - node->getWorldPosition();
	to_target.z = 0.0;
	bool near_to_target = to_target.length2() < target_reach_distance.get() * target_reach_distance.get();

	if (!target_reached && near_to_target)
	{
		target_reached = true;

		if (use_target_node && target)
		{
			if (auto target_component = getComponent<PathfindingTarget>(target))
				target_component->onReached();
		}

		if (has_fixed_target)
			has_fixed_target = false;
	}

	if (target_reached)
		return;

	if (corridor->getNumNextCorners() == 0)
		return;

	// steer toward the next corner at full speed: the avoidance solver needs
	// the raw intent; slowing into the turns is done by moveWithVelocity
	vec3 direction = vec3(corridor->getNextCorner(0) - node->getWorldPosition());
	direction.z = 0.0f;
	if (direction.length2() <= Consts::EPS)
		return;

	desired_velocity = normalize(direction) * movement_speed;
}

void ExperimentalSeeker::moveWithVelocity(const vec3 &new_velocity, float ifps)
{
	command_velocity = new_velocity;

	if (!corridor || !corridor->isValid())
	{
		velocity = vec3_zero;
		return;
	}

	Vec3 old_position = node->getWorldPosition();

	// slow down while turning: the speed grows as the agent lines up
	vec3 applied_velocity = new_velocity;
	vec3 direction = new_velocity;
	direction.z = 0.0f;
	if (direction.length2() > Consts::EPS)
	{
		direction = normalize(direction);
		applied_velocity *= saturate(1.0f - getAngle(node->getWorldRotation(), rotationFromDir(direction)) * (1.0f / 180.0f));
		node->setWorldRotation(rotateTowards(node->getWorldRotation(), rotationFromDir(direction), rotation_speed * ifps), true);
	}

	corridor->movePosition(old_position + Vec3(applied_velocity * ifps));
	place_node(corridor->getPosition());

	velocity = ifps > Consts::EPS ? vec3((node->getWorldPosition() - old_position) / ifps) : vec3_zero;
}

// Moves the agent to the given position (e.g. after crowd de-penetration),
// constrained to the navigation mesh.
void ExperimentalSeeker::correctPosition(const Vec3 &position)
{
	if (!corridor || !corridor->isValid())
		return;

	corridor->movePosition(position);
	place_node(corridor->getPosition());
}

// Teleports the agent to the given position on the navigation mesh and
// forces a replan from there.
void ExperimentalSeeker::teleport(const Vec3 &position)
{
	if (!corridor)
		return;

	auto mesh = corridor->getNavigationMesh();
	if (!mesh)
		mesh = navigation_mesh;
	if (!mesh)
		return;

	Vec3 point = position;
	mesh->findNearestPoint(filter, position, point);
	long long polygon = mesh->findNearestPolygon(filter, point);
	if (!mesh->isPolygonValid(polygon))
		return;

	corridor->teleport(mesh, polygon, point);
	place_node(corridor->getPosition());

	waypoints.clear();
	route_status = ExperimentalNavigationPath::STATUS_NO_PATH;
	stall_position = node->getWorldPosition();
	stall_timer = 0.0f;
}

bool ExperimentalSeeker::resolve_target_position(Vec3 &ret_position) const
{
	if (use_target_node)
	{
		if (!target)
			return false;
		ret_position = target->getWorldPosition();
		return true;
	}

	if (!has_fixed_target)
		return false;
	ret_position = fixed_target_position;
	return true;
}

// Places the node at the given route position, snapped to the ground below:
// the navigation mesh only approximates the floor height.
void ExperimentalSeeker::place_node(const Vec3 &position)
{
	Vec3 placed = position;
	if (snap_to_ground)
	{
		// the probe covers the height error of the bake
		Vec3 from = position + Vec3(0.0, 0.0, 0.5);
		Vec3 to = position - Vec3(0.0, 0.0, 1.0);
		if (World::getIntersection(from, to, ground_intersection_mask.get(), ground_exclude, ground_hit))
			placed.z = ground_hit->getPoint().z;
	}
	node->setWorldPosition(placed);
}

void ExperimentalSeeker::update_route(float ifps)
{
	if (!corridor)
		return;

	Vec3 target_position;
	if (!resolve_target_position(target_position))
		return;

	replan_timer = Math::max(replan_timer - ifps, 0.0f);
	global_replan_timer = Math::max(global_replan_timer - ifps, 0.0f);
	shortcut_timer = Math::max(shortcut_timer - ifps, 0.0f);

	if (global_navigation_mesh)
	{
		update_global_route(target_position);
		update_waypoint_shortcut();
	}

	// the local goal: the current waypoint of the global route, or the target
	// itself when there is no low resolution mesh
	Vec3 local_target = target_position;
	if (global_navigation_mesh && !current_waypoint(local_target))
	{
		// no waypoints while the global plan rebuilds: keep the corridor; with
		// no global plan at all, walk directly as far as the loaded tiles allow
		if (corridor->isValid() && (global_status != ExperimentalNavigationPath::STATUS_NO_PATH || global_replan_timer > 0.0f))
			local_target = planned_local_target;
		else
			local_target = target_position;
	}

	// no movement while trying to move: the route is stale, rebuild it
	bool stalled = update_stall(ifps);

	// replan when the route is incomplete, the corridor was invalidated, the
	// goal has moved or the agent stalled; failures retry with a cooldown
	bool local_target_moved = (local_target - planned_local_target).length2() > Consts::EPS;
	if (route_status != ExperimentalNavigationPath::STATUS_COMPLETE || !corridor->isValid() || local_target_moved || stalled)
	{
		if (local_target_moved || stalled || replan_timer <= 0.0f)
		{
			replan_route(local_target);
			target_reached = false;
		}
	}
}

// Reports a stall: the agent wants to move (the intent of the previous frame)
// but has stayed within its own radius for stall_replan_time.
bool ExperimentalSeeker::update_stall(float ifps)
{
	if (stall_replan_time.get() <= 0.0f)
		return false;

	Vec3 moved = node->getWorldPosition() - stall_position;
	moved.z = 0.0;
	float stall_distance = Math::max(agent_radius.get(), 0.1f);
	bool wants_to_move = !target_reached && desired_velocity.length2() > Consts::EPS;
	if (!wants_to_move || moved.length2() > Scalar(stall_distance * stall_distance))
	{
		stall_position = node->getWorldPosition();
		stall_timer = 0.0f;
		return false;
	}

	stall_timer += ifps;
	if (stall_timer < stall_replan_time.get())
		return false;

	stall_position = node->getWorldPosition();
	stall_timer = 0.0f;
	return true;
}

// Builds the global plan and rebuilds it for a moved goal or after a dropped
// plan, rate limited.
void ExperimentalSeeker::update_global_route(const Vec3 &target_position)
{
	bool target_moved = (target_position - planned_target_position).length2() > Consts::EPS;
	if (waypoints.size() > 0 && !target_moved)
		return;
	if (global_replan_timer > 0.0f)
		return;

	replan_global_route(target_position);
	global_replan_timer = global_replan_retry_time.get();
}

void ExperimentalSeeker::replan_global_route(const Vec3 &target_position)
{
	planned_target_position = target_position;
	waypoints.clear();
	waypoint_index = 0;
	global_status = ExperimentalNavigationPath::STATUS_NO_PATH;

	// see the start note in replan_route
	Vec3 start = corridor->isValid() ? corridor->getPosition() : node->getWorldPosition();
	global_fetch->fetchForce(start, target_position);
	auto route = global_fetch->takePath();
	global_failure = route ? route->getFailureReason() : ExperimentalNavigationPath::FAILURE_DATA_MISSING;
	if (!route || route->getStatus() == ExperimentalNavigationPath::STATUS_NO_PATH || route->getNumPoints() < 2)
		return;
	global_status = route->getStatus();

	// split the long segments so every local leg stays in the streamed range
	float spacing = Math::max(waypoint_spacing.get(), 1.0f);
	Vec3 previous = route->getPoint(0);
	for (int i = 1; i < route->getNumPoints(); i++)
	{
		Vec3 point = route->getPoint(i);
		int splits = (int)Math::floor((float)length(point - previous) / spacing);
		for (int k = 1; k <= splits; k++)
			waypoints.append(lerp(previous, point, Scalar(k) / (splits + 1)));
		waypoints.append(point);
		previous = point;
	}

	// a complete route ends at the exact target instead of the snapped point
	if (global_status == ExperimentalNavigationPath::STATUS_COMPLETE)
		waypoints.last() = target_position;
}

// Jumps the current waypoint to a closer one ahead: a local shortcut may
// leave the agent closer to a later waypoint, and walking back to touch the
// current one first is the backtracking this removes. Candidates are tried
// nearest first, the first complete route wins; the fetches are rate limited.
void ExperimentalSeeker::update_waypoint_shortcut()
{
	if (waypoint_index + 1 >= waypoints.size())
		return;

	Vec3 position = node->getWorldPosition();
	Vec3 to_current = waypoints[waypoint_index] - position;
	to_current.z = 0.0;
	Scalar current_distance2 = to_current.length2();

	Vector<int> candidates;
	for (int i = waypoint_index + 1; i < waypoints.size(); i++)
	{
		Vec3 to_waypoint = waypoints[i] - position;
		to_waypoint.z = 0.0;
		if (to_waypoint.length2() < current_distance2)
			candidates.append(i);
	}

	// the current waypoint is the nearest one: nothing to jump to
	if (candidates.size() == 0)
		return;

	if (shortcut_timer > 0.0f)
		return;
	shortcut_timer = global_replan_retry_time.get();

	// see the start note in replan_route
	Vec3 start = corridor->isValid() ? corridor->getPosition() : position;

	while (candidates.size() > 0)
	{
		int best = 0;
		Scalar best_distance2 = current_distance2;
		for (int k = 0; k < candidates.size(); k++)
		{
			Vec3 to_waypoint = waypoints[candidates[k]] - position;
			to_waypoint.z = 0.0;
			Scalar distance2 = to_waypoint.length2();
			if (distance2 < best_distance2)
			{
				best = k;
				best_distance2 = distance2;
			}
		}

		int candidate = candidates[best];
		candidates.remove(best);

		route_fetch->fetchForce(start, waypoints[candidate]);
		auto route = route_fetch->takePath();
		if (route && route->getStatus() == ExperimentalNavigationPath::STATUS_COMPLETE)
		{
			// forward only and self-clearing: the new current waypoint is the
			// nearest one; the regular replan rebuilds the corridor
			waypoint_index = candidate;
			return;
		}
	}
}

// Skips the waypoints that are already passed and returns the current one.
bool ExperimentalSeeker::current_waypoint(Vec3 &ret_waypoint)
{
	float reach = Math::min(waypoint_reach_distance.get(), waypoint_spacing.get() * 0.5f);
	while (waypoint_index + 1 < waypoints.size())
	{
		Vec3 to_waypoint = waypoints[waypoint_index] - node->getWorldPosition();
		to_waypoint.z = 0.0;
		if (to_waypoint.length2() > Scalar(reach * reach))
			break;
		waypoint_index++;
	}

	if (waypoints.size() == 0)
		return false;
	ret_waypoint = waypoints[waypoint_index];
	return true;
}

void ExperimentalSeeker::replan_route(const Vec3 &local_target)
{
	planned_local_target = local_target;

	// plan from the position on the navigation mesh: the node itself may be
	// snapped to the ground away from the mesh height
	Vec3 start = corridor->isValid() ? corridor->getPosition() : node->getWorldPosition();
	route_fetch->fetchForce(start, local_target);
	auto route = route_fetch->takePath();

	route_status = route ? route->getStatus() : ExperimentalNavigationPath::STATUS_NO_PATH;
	route_failure = route ? route->getFailureReason() : ExperimentalNavigationPath::FAILURE_DATA_MISSING;
	if (route_status == ExperimentalNavigationPath::STATUS_NO_PATH || !corridor->setPath(route))
	{
		route_status = ExperimentalNavigationPath::STATUS_NO_PATH;
		corridor->clear();

		// a waypoint on a cut out or unloaded spot is skipped; failing the
		// last one drops the plan so the global route is rebuilt
		bool waypoint_failed = route_failure == ExperimentalNavigationPath::FAILURE_TARGET_OUTSIDE
			|| route_failure == ExperimentalNavigationPath::FAILURE_DISCONNECTED
			|| route_failure == ExperimentalNavigationPath::FAILURE_OBSTACLE_BLOCKED;
		if (waypoint_failed)
		{
			if (waypoint_index + 1 < waypoints.size())
				waypoint_index++;
			else
				waypoints.clear();
		}
	}

	if (route_status == ExperimentalNavigationPath::STATUS_COMPLETE)
		replan_timer = 0.0f;
	else
		replan_timer = replan_retry_time.get();
}
