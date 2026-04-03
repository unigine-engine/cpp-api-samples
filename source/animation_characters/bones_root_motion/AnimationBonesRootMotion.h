#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates the root motion technique.
// Root motion extracts movement from an animation's root bone and applies it
// to the object's world position, resulting in animation-driven locomotion.
// This ensures that character movement perfectly matches the animation.
class AnimationBonesRootMotion : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesRootMotion, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference model for comparison (plays animation without root motion)
	PROP_PARAM(Node, skeleton_pose_node);
	// Model that uses root motion (object moves based on animation)
	PROP_PARAM(Node, skeleton_pose_root_motion_node);

private:
	void init();
	void update();
	void post_update();
	void shutdown();

private:
	Unigine::NodeSkeletonPosePtr pose;

	Unigine::NodeSkeletonPosePtr pose_root_motion;
	Unigine::AnimScriptPtr anim_script;
};
