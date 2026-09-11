#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates basic Inverse Kinematics (IK) with a pole vector.
// Two manipulators are provided: one for the IK target (where the end effector should reach)
// and one for the pole vector (which controls the bending direction of joints).
class AnimationBonesIKSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesIKSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to NodeSkeletonPose node containing the animation script
	PROP_PARAM(Node, skeleton_pose_node);
	PROP_PARAM(String, root_joint_name);

private:
	void init();
	void update();
	void shutdown();

private:
	// Manipulator controlling where the end of the IK chain should reach
	Unigine::WidgetManipulatorTranslatorPtr target_translator;
	// Manipulator controlling the pole vector (defines the bending plane for joints)
	Unigine::WidgetManipulatorTranslatorPtr pole_translator;

	Unigine::NodeSkeletonPosePtr skeleton_pose;
	Unigine::AnimScriptPtr anim_script;
	int root_joint{-1};
};
