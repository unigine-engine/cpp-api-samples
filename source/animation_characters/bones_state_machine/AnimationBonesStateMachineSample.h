#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates animation state machines for character animation.
// Three different state machine configurations are shown:
// 1. Walk machine: speed-based walk animation with turn-around capability
// 2. Idle turn machine: idle state with turning based on input direction
// 3. Walk/run machine: 2D blend space for locomotion (forward/backward + strafe)
// Each state machine uses root motion to drive character movement.
class AnimationBonesStateMachineSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesStateMachineSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Walk state machine: transitions between idle and walk based on speed variable
	PROP_PARAM(Node, skeleton_pose_node_walk);

	// Idle turn state machine: 1D blend between turn left, idle, and turn right
	PROP_PARAM(Node, skeleton_pose_node_idle_turn);

	// Walk/run state machine: 2D blend space for directional locomotion
	PROP_PARAM(Node, skeleton_pose_node_walk_run);

private:
	void init();
	void update();

private:
	Unigine::NodeSkeletonPosePtr pose_walk;
	Unigine::AnimScriptPtr walk_script;

	Unigine::NodeSkeletonPosePtr pose_idle_turn;
	Unigine::AnimScriptPtr idle_turn_script;

	Unigine::NodeSkeletonPosePtr pose_walk_run;
	Unigine::AnimScriptPtr walk_run_script;
};
