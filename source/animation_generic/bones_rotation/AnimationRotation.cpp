// Demonstrates joint rotation with additive animation blending. An idle animation
// plays continuously while left/right shoot animations are additively blended.
// A horizontal joint bone is rotated programmatically each frame.

#include "AnimationRotation.h"

#include <UnigineGame.h>
#include <UnigineLog.h>

REGISTER_COMPONENT(AnimationRotation);

using namespace Unigine;
using namespace Math;

void AnimationRotation::init()
{
	// The component drives the joints of its own node, so any other node type is a setup error
	skeleton_pose = checked_ptr_cast<NodeSkeletonPose>(node);
	if (skeleton_pose.isValid() == false)
	{
		Log::error("AnimationRotation::init(): the component must be assigned to a NodeSkeletonPose node\n");
		return;
	}

	// Store initial bone transform for rotation accumulation
	bone_transform = skeleton_pose->getJointTransform(horizontal_joint);
}

// Idle animation is updated, shoot animation is blended, and bone is rotated each frame.
// The joint is rotated in post update, after the animation graph has written its pose,
// otherwise the played animation would overwrite the rotation.
void AnimationRotation::post_update()
{
	if (skeleton_pose.isValid() == false)
		return;

	// Rotate horizontal joint bone continuously around Z axis
	bone_transform = bone_transform * rotateZ(rotation_speed * Game::getIFps());
	skeleton_pose->setJointTransform(horizontal_joint, bone_transform);
	skeleton_pose->forceApplyPose();
}
