#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates LookAt chains for aiming bones at a target.
// Multiple bones (e.g., spine and head) can participate in the chain with different weights,
// distributing the rotation across the chain for a natural look.
class AnimationBonesLookAtSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesLookAtSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to NodeSkeletonPose node containing the animation script
	PROP_PARAM(Node, skeleton_pose_node);
	PROP_PARAM(String, last_joint_name);

private:
	void init();
	void update();
	void shutdown();

private:
	// Manipulator for the look-at target position
	Unigine::WidgetManipulatorTranslatorPtr target_translator;
	// Manipulator for the pole vector (controls the "up" orientation of the chain)
	Unigine::WidgetManipulatorTranslatorPtr pole_translator;

	Unigine::NodeSkeletonPosePtr skeleton_pose;
	Unigine::AnimScriptPtr anim_script;
	int last_joint{-1};
};
