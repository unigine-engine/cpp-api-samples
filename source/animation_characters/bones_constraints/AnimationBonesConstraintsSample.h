#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates Inverse Kinematics (IK) with bone rotation constraints.
// A target can be dragged in 3D space, and the bone chain automatically adapts
// while respecting the specified angle limits for yaw/pitch/roll axes.
class AnimationBonesConstraintsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesConstraintsSample, ComponentBase);
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
	// Widget manipulator for interactive dragging of the IK target in 3D space
	Unigine::WidgetManipulatorTranslatorPtr target_translator;

	Unigine::NodeSkeletonPosePtr skeleton_pose;
	Unigine::AnimScriptPtr anim_script;
	int last_joint{-1};
};
