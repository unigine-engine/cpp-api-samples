#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates foot placement on uneven surfaces using IK chains.
// Raycasting is used to detect ground contact: each frame, a ray is cast downward
// from each foot to find the surface. The foot's position and rotation are then
// adjusted to match the surface height and normal.
class AnimationBonesSimpleFootPlacementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesSimpleFootPlacementSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to NodeSkeletonPose node containing the animation script
	PROP_PARAM(Node, skeleton_pose_node);
	PROP_PARAM(String, left_foot_name);
	PROP_PARAM(String, right_foot_name);
	// Height offset from ground to foot pivot point
	PROP_PARAM(Float, foot_height, 0.1f);
	// Optional movable platform/obstacle for testing foot placement
	PROP_PARAM(Node, obstacle);

private:
	enum FOOT
	{
		FOOT_LEFT = 0,
		FOOT_RIGHT,
		FOOT_NUM,
	};

private:
	void init();
	void update();
	void shutdown();

private:
	// Manipulators for moving/rotating the obstacle platform
	Unigine::WidgetManipulatorTranslatorPtr translator;
	Unigine::WidgetManipulatorRotatorPtr rotator;

	Unigine::NodeSkeletonPosePtr skeleton_pose;
	Unigine::AnimScriptPtr anim_script;
	int chain_ids[FOOT_NUM];
	// Last joint in each chain (the foot bone used as end effector)
	int effector_joints[FOOT_NUM];
	Unigine::Math::quat src_effector_rotations[FOOT_NUM];
};
