#include "AnimationBonesConstraintsSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesConstraintsSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesConstraintsSample::init()
{
	skeleton_pose = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node.get());
	if (skeleton_pose.isValid() == false)
	{
		Log::error("AnimationBonesConstraintsSample::init(): skinned is null\n");
		return;
	}

	anim_script = skeleton_pose->getAnimScript();
	if (anim_script.isValid() == false)
	{
		Log::error("AnimationBonesConstraintsSample::init(): animation script is null\n");
		return;
	}

	ConstSkeletonPtr skeleton = skeleton_pose->getSkeleton();
	if (skeleton.isValid())
		last_joint = skeleton->findJoint(last_joint_name.get());

	// Create a 3D manipulator widget for interactive IK target positioning
	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(anim_script->getParamVec3("ik_target"))));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	// Enable debug visualization
	Visualizer::setEnabled(true);
	// Disable depth test so visualizations are always visible (even through geometry)
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesConstraintsSample::update()
{
	if (anim_script.isNull())
		return;

	PlayerPtr player = Game::getPlayer();
	if (player.isValid() == false)
		return;

	// Sync the manipulator widget with the current camera so it renders correctly
	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	// Update IK target position based on current manipulator position
	Vec3 pos = target_translator->getTransform().getTranslate();
	anim_script->setParamVec3("ik_target", vec3(pos));

	Visualizer::renderMessage3D(pos + Vec3(0.f,0.f,-0.1f), vec3_zero, "Drag Me", vec4_white, 1, 30);

	Visualizer::renderSolidSphere(0.02f, translate(pos), vec4_green);

	if (0 < skeleton_pose->getNumLayers())
	{
		skeleton_pose->renderLayerBones(0, skeleton_pose->getWorldTransform(), vec4_black);

		if (last_joint != -1)
		{
			Vec3 last_joint_pos(skeleton_pose->getLayerJointObjectTransform(0, last_joint).getTranslate());
			last_joint_pos = skeleton_pose->getWorldTransform() * last_joint_pos;

			Visualizer::renderLine3D(last_joint_pos, pos, last_joint_pos, vec4_red);
		}
	}
}

void AnimationBonesConstraintsSample::shutdown()
{
	// Restore default visualizer settings
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
