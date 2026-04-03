#include "AnimationBonesRootMotion.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(AnimationBonesRootMotion);

using namespace Unigine;
using namespace Math;

void AnimationBonesRootMotion::init()
{
	pose = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node.get());
	if (pose.isValid() == false)
	{
		Log::error("AnimationBonesRootMotion::init(): pose is null\n");
		return;
	}

	pose_root_motion = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_root_motion_node.get());
	if (pose_root_motion.isValid() == false)
	{
		Log::error("AnimationBonesRootMotion::init(): root motion pose is null\n");
		return;
	}

	anim_script = pose_root_motion->getAnimScript();
	if (anim_script.isValid() == false)
	{
		Log::error("AnimationBonesRootMotion::init(): anim script is null\n");
		return;
	}

	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesRootMotion::update()
{
	// Render coordinate axes at each character's position for visual reference
	auto render_transform = [](const Mat4 t)
	{
		Vec3 p = t.getTranslate();
		Visualizer::renderVector(p, p + t.getAxisX(), vec4_red);
		Visualizer::renderVector(p, p + t.getAxisY(), vec4_green);
		Visualizer::renderVector(p, p + t.getAxisZ(), vec4_blue);
	};

	render_transform(pose->getWorldTransform());
	render_transform(pose_root_motion->getWorldTransform());
}

void AnimationBonesRootMotion::post_update()
{
	// Apply accumulated root motion to move the object in world space.
	// Done in post_update to ensure animation processing is complete.
	if (anim_script->isActiveRootMotion())
	{
		Transform delta = anim_script->getRootMotionDelta();
		pose_root_motion->setWorldTransform(pose_root_motion->getWorldTransform() * Mat4(delta.getMat()));
	}
}

void AnimationBonesRootMotion::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
