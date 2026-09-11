#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineNodes.h>

// Combines idle animation with alternating shoot animations and programmatic bone rotation.
class AnimationRotation : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationRotation, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_POST_UPDATE(post_update);

private:
	void init();
	void post_update();

private:
	// Skeleton pose node for animation playback
	Unigine::NodeSkeletonPosePtr skeleton_pose;

	// Accumulated world transform for rotating bone
	Unigine::Math::mat4 bone_transform = Unigine::Math::mat4_identity;

	// Bone index for horizontal rotation joint
	const int horizontal_joint = 1;
	// Turn speed of the joint, degrees per second
	const float rotation_speed = 45.0f;
};
