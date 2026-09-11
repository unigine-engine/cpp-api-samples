#include "ExperimentalSeekerCharacter.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(ExperimentalSeekerCharacter);

using namespace Unigine;
using namespace Unigine::Math;

static NodeSkeletonPosePtr find_skeleton_pose(const NodePtr &node)
{
	NodeSkeletonPosePtr pose = checked_ptr_cast<NodeSkeletonPose>(node);
	if (pose)
		return pose;

	for (int i = 0; i < node->getNumChildren(); i++)
	{
		pose = find_skeleton_pose(node->getChild(i));
		if (pose)
			return pose;
	}
	return nullptr;
}

void ExperimentalSeekerCharacter::init()
{
	ExperimentalSeeker::init();

	debug_visualizer_height = 1.9f;

	pose = skeleton_pose.get() ? checked_ptr_cast<NodeSkeletonPose>(skeleton_pose.get()) : find_skeleton_pose(node);
	if (!pose)
	{
		Log::warning("ExperimentalSeekerCharacter::init(): no NodeSkeletonPose, the seeker falls back to the direct velocity movement\n");
		return;
	}

	anim_script = pose->getAnimScript();
	if (!anim_script)
	{
		Log::warning("ExperimentalSeekerCharacter::init(): the skeleton pose has no anim script, the seeker falls back to the direct velocity movement\n");
		return;
	}

	// the locomotion states are searched in the first state machine of the graph
	if (anim_script->getNumStateMachines() > 0)
		locomotion_sm = 0;

	has_moving = has_anim_param(moving_param.get());
	has_rate = has_anim_param(rate_param.get());
	has_turn_amount = has_anim_param(turn_amount_param.get());

	if (!has_moving)
		Log::warning("ExperimentalSeekerCharacter::init(): the anim script has no \"%s\" parameter, the seeker falls back to the direct velocity movement\n", moving_param.get());

	// the graph contract is checked once: no watchdogs at runtime
	if (has_moving && (!graph_has_state("idle") || !graph_has_state("stop")))
		Log::warning("ExperimentalSeekerCharacter::init(): the state machine has no \"idle\" or \"stop\" state, the locomotion may misread the graph\n");
	if (has_turn_amount && !graph_has_state("turn"))
	{
		Log::warning("ExperimentalSeekerCharacter::init(): the state machine has no \"turn\" state, the turns are done by rotating the node\n");
		has_turn_amount = false;
	}
}

// The post-update: applies the root motion delta, constrained by the
// corridor and snapped to the ground; the velocity is measured from the
// applied movement for the avoidance solver.
void ExperimentalSeekerCharacter::apply_root_motion()
{
	if (!anim_script || !has_moving)
		return;

	if (!anim_script->isActiveRootMotion() || !corridor || !corridor->isValid())
	{
		velocity = vec3_zero;
		return;
	}

	float ifps = Game::getIFps();
	Transform delta = anim_script->getRootMotionDelta();

	// the delta is defined in the skeleton pose space: move the pose and keep
	// the component node rigidly attached to it
	Mat4 old_root = node->getWorldTransform();
	Mat4 pose_to_root = inverse(pose->getWorldTransform()) * old_root;
	Mat4 new_root = pose->getWorldTransform() * Mat4(delta.getMat()) * pose_to_root;

	corridor->movePosition(new_root.getTranslate());
	new_root.setColumn3(3, corridor->getPosition());
	node->setWorldTransform(new_root);
	place_node(corridor->getPosition());

	velocity = ifps > Consts::EPS ? vec3((node->getWorldPosition() - old_root.getTranslate()) / ifps) : vec3_zero;
}

// Zeroed while turning in place: the crowd must see a parked agent. The other
// states keep the intent, or a blocked agent would never be let through.
void ExperimentalSeekerCharacter::computeControl()
{
	ExperimentalSeeker::computeControl();

	if (control_state == CONTROL_TURN)
		desired_velocity = vec3_zero;
}

// Drives the state machine and the graph parameters; the node is not
// translated here, the movement comes from apply_root_motion. The solver
// command steers the walk, the states are switched by the stable route
// intent and the command magnitude - never by the command direction, so an
// avoidance detour cannot flap the regime.
void ExperimentalSeekerCharacter::moveWithVelocity(const vec3 &new_velocity, float ifps)
{
	command_velocity = new_velocity;

	// without a proper animation setup behave as a regular seeker
	if (!anim_script || !has_moving)
	{
		ExperimentalSeeker::moveWithVelocity(new_velocity, ifps);
		return;
	}

	// the flattened command and intent with the course errors to them
	vec3 command = new_velocity;
	command.z = 0.0f;
	vec3 intent = desired_velocity;
	intent.z = 0.0f;
	bool on_route = corridor && corridor->isValid();
	bool has_intent = on_route && intent.length2() > Consts::EPS;
	bool has_command = on_route && command.length2() > Consts::EPS;
	float intent_angle = has_intent ? course_error(intent) : 0.0f;
	float command_angle = has_command ? course_error(command) : 0.0f;

	// the command is bounded by max_acceleration around the current velocity,
	// so the stop signal is read relative to the speed reachable right now
	vec3 ground_velocity = velocity;
	ground_velocity.z = 0.0f;
	float reachable_speed = movement_speed;
	if (max_acceleration > 0.0f)
		reachable_speed = min(reachable_speed, ground_velocity.length() + max_acceleration * ifps);
	bool stop_signal = command.length() < reachable_speed * yield_ratio;

	// the route turned beyond the threshold of the animated turn
	bool route_turn = has_intent && Math::abs(intent_angle) > turn_angle;

	Locomotion body = read_locomotion();

	// committed transitions: a stop plays to its foot phase, a turn is
	// finished by the graph. The arrival needs no handling - the base zeroes
	// the intent within target_reach_distance, which reads as a stop
	switch (control_state)
	{
		case CONTROL_WALK:
			if (!has_intent || route_turn || stop_signal)
				control_state = CONTROL_STOPPING;
			else if (has_command)
				steer(command, ifps);
			break;

		case CONTROL_STOPPING:
			if (body.resting)
				control_state = CONTROL_REST;
			else if (has_intent && !route_turn)
				steer(intent, ifps);
			break;

		case CONTROL_REST:
			if (route_turn && has_turn_amount)
			{
				// negated: a positive angle points left, and the left turns
				// occupy the negative half of the blend axis
				turn_request.amount = clamp(-intent_angle / 180.0f, -1.0f, 1.0f);
				turn_request.seen = false;
				control_state = CONTROL_TURN;
			}
			else if (route_turn)
				steer(intent, ifps); // the fallback pivot without the turn feature
			else if (has_intent && !stop_signal)
				control_state = CONTROL_WALK;
			break;

		case CONTROL_TURN:
			// the graph decides when the turn is over
			if (body.turning)
				turn_request.seen = true;
			if (turn_request.seen && !body.turning)
				control_state = CONTROL_REST;
			break;
	}

	anim_script->setParamBool(moving_param.get(), control_state == CONTROL_WALK);
	if (has_turn_amount)
		anim_script->setParamFloat(turn_amount_param.get(), control_state == CONTROL_TURN ? turn_request.amount : 0.0f);
	update_playback_rate(command_angle);
}

const char *ExperimentalSeekerCharacter::getControlStateName() const
{
	switch (control_state)
	{
		case CONTROL_WALK: return "walk";
		case CONTROL_STOPPING: return "stopping";
		case CONTROL_REST: return "rest";
		case CONTROL_TURN: return "turn";
	}
	return "";
}

const char *ExperimentalSeekerCharacter::getLocomotionStateName() const
{
	if (!anim_script || locomotion_sm == -1)
		return "";

	const char *state = anim_script->getStateMachineCurrentStateName(locomotion_sm);
	return state ? state : "";
}

// The body state: the velocity measured on the previous frame and the
// current state of the graph.
ExperimentalSeekerCharacter::Locomotion ExperimentalSeekerCharacter::read_locomotion() const
{
	vec3 ground_velocity = velocity;
	ground_velocity.z = 0.0f;
	bool standing = ground_velocity.length() < standing_speed;
	bool stopping = state_has("stop");

	Locomotion state;
	state.turning = state_has("turn");
	state.resting = (state_has("idle") && !state.turning) || (stopping && standing);
	return state;
}

// Signed angle (in degrees) from the character forward (+Y axis) to the
// direction: positive means the direction points to the left.
float ExperimentalSeekerCharacter::course_error(const vec3 &direction) const
{
	vec3 forward = vec3(node->getWorldTransform().getAxisY());
	return Math::atan2(forward.x * direction.y - forward.y * direction.x, forward.x * direction.x + forward.y * direction.y) * Consts::RAD2DEG;
}

// The rate makes the walk root motion match movement_speed and drops with
// the course error, the way the base seeker slows into the turns; never
// below min_rate.
void ExperimentalSeekerCharacter::update_playback_rate(float angle)
{
	if (!has_rate)
	{
		rate = 1.0f;
		return;
	}

	float desired_speed = movement_speed * saturate(1.0f - Math::abs(angle) * (1.0f / 180.0f));
	rate = max(desired_speed / max(animation_speed.get(), Consts::EPS), min_rate.get());
	anim_script->setParamFloat(rate_param.get(), rate);
}

// The walk is straight, so the node is rotated toward the direction at
// rotation_speed; rotateTowards clamps at the target and cannot overshoot.
void ExperimentalSeekerCharacter::steer(const vec3 &direction, float ifps)
{
	node->setWorldRotation(rotateTowards(node->getWorldRotation(), rotationFromDir(normalize(direction)), rotation_speed * ifps), true);
}

bool ExperimentalSeekerCharacter::state_has(const char *word) const
{
	if (!anim_script || locomotion_sm == -1)
		return false;

	const char *state = anim_script->getStateMachineCurrentStateName(locomotion_sm);
	return state && String(state).find(word) != -1;
}

bool ExperimentalSeekerCharacter::graph_has_state(const char *word) const
{
	if (!anim_script || locomotion_sm == -1)
		return false;

	for (int i = 0; i < anim_script->getStateMachineNumStates(locomotion_sm); i++)
	{
		const char *state = anim_script->getStateMachineStateName(locomotion_sm, i);
		if (state && String(state).find(word) != -1)
			return true;
	}
	return false;
}

bool ExperimentalSeekerCharacter::has_anim_param(const char *name) const
{
	if (!anim_script || String::isEmpty(name))
		return false;

	for (int i = 0; i < anim_script->getNumParams(); i++)
	{
		if (String::equal(anim_script->getParamName(i), name))
			return true;
	}
	return false;
}
