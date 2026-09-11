#pragma once

#include "ExperimentalSeeker.h"

#include <UnigineNodes.h>

// An ExperimentalSeeker for animated characters: the velocity command drives
// a small state machine (walk, stopping, rest, turn) through the animation
// graph parameters, and the movement itself comes from the root motion.
class ExperimentalSeekerCharacter : public ExperimentalSeeker
{
public:
	COMPONENT_DEFINE(ExperimentalSeekerCharacter, ExperimentalSeeker);
	COMPONENT_INIT(init);
	// before the debug visualizer of the base, which runs at the order 1
	COMPONENT_POST_UPDATE(apply_root_motion);

	PROP_GROUP("Animation Graph");
	PROP_TOOLTIP("The node with the NodeSkeletonPose; searched in the hierarchy of the agent when empty");
	PROP_PARAM(Node, skeleton_pose);
	PROP_TOOLTIP("Bool graph parameter: true while the character wants to walk; an empty name falls back to the direct velocity movement");
	PROP_PARAM(String, moving_param, "is_moving");
	PROP_TOOLTIP("Float graph parameter: the playback rate of the locomotion animations; an empty name keeps the rate 1");
	PROP_PARAM(String, rate_param, "rate");
	PROP_TOOLTIP("Float graph parameter: the turn-in-place request, -1..1 of the half circle, negative - left; an empty name turns the node directly");
	PROP_PARAM(String, turn_amount_param, "turn_amount");

	PROP_GROUP("Locomotion");
	PROP_TOOLTIP("Root motion speed of the walk clip at the rate 1, m/s: the locomotion plays at the rate movement_speed / animation_speed");
	PROP_PARAM(Float, animation_speed, 1.34f);
	PROP_TOOLTIP("Floor of the playback rate, must stay above zero: a frozen walk never reaches the foot phase of the stop");
	PROP_PARAM(Float, min_rate, 0.5f);
	PROP_TOOLTIP("Measured speed below which the character counts as standing, m/s: a standing character may turn in place or start walking");
	PROP_PARAM(Float, standing_speed, 0.3f);
	PROP_TOOLTIP("Angular error of the route separating the two course corrections, deg: below - the node is rotated, above - the character stops and turns in place");
	PROP_PARAM(Float, turn_angle, 90.0f);
	PROP_TOOLTIP("How readily the agent yields to the crowd: the walk is held while the solver commands less than this fraction of the currently reachable speed (see max_acceleration), and started or resumed above it");
	PROP_PARAM(Float, yield_ratio, 0.25f);

	// zeroed while turning in place: the crowd must see a parked agent
	void computeControl() override;
	// drives the state machine and the graph parameters; the movement itself
	// is applied in post-update from the root motion delta
	void moveWithVelocity(const Unigine::Math::vec3 &new_velocity, float ifps) override;

	// current states for the sample UI
	const char *getControlStateName() const;
	const char *getLocomotionStateName() const;
	// the playback rate currently requested
	float getPlaybackRate() const { return rate; }

private:
	enum CONTROL_STATE
	{
		CONTROL_WALK,     // following the solver command
		CONTROL_STOPPING, // committed to stop, waiting for the foot phase
		CONTROL_REST,     // standing, deciding what to do next
		CONTROL_TURN,     // turning in place, the graph rotates the character
	};

	// the body state as seen from the animation graph
	struct Locomotion
	{
		bool turning{false}; // the state machine is in a turn state
		bool resting{false}; // idle or stopped standing: free to turn or start
	};

	// the turn-in-place request: the latched blend amount and whether the
	// graph has picked the turn up
	struct TurnRequest
	{
		float amount{0.0f};
		bool seen{false};
	};

	void init();
	void apply_root_motion();

	Locomotion read_locomotion() const;
	float course_error(const Unigine::Math::vec3 &direction) const;
	void update_playback_rate(float angle);
	void steer(const Unigine::Math::vec3 &direction, float ifps);

	// the current state of the state machine has the given word in its name
	bool state_has(const char *word) const;
	// the state machine has a state with the given word in its name
	bool graph_has_state(const char *word) const;
	bool has_anim_param(const char *name) const;

	Unigine::NodeSkeletonPosePtr pose;
	Unigine::AnimScriptPtr anim_script;
	int locomotion_sm{-1};
	bool has_moving{false};
	bool has_rate{false};
	bool has_turn_amount{false};

	CONTROL_STATE control_state{CONTROL_REST};
	TurnRequest turn_request;

	float rate{1.0f};
};
