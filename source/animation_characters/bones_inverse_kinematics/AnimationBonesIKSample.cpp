#include "AnimationBonesIKSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesIKSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesIKSample::init()
{
	skeleton_pose = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node.get());
	if (skeleton_pose.isValid() == false)
	{
		Log::error("AnimationBonesIKSample::init(): skeleton pose is null\n");
		return;
	}

	anim_script = skeleton_pose->getAnimScript();
	if (anim_script.isValid() == false)
	{
		Log::error("AnimationBonesIKSample::init(): animation script is null\n");
		return;
	}

	ConstSkeletonPtr skeleton = skeleton_pose->getSkeleton();
	if (skeleton.isValid())
		root_joint = skeleton->findJoint(root_joint_name.get());

	// Create manipulator for the IK target (end effector goal position)
	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(anim_script->getParamVec3("ik_target"))));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	// Create manipulator for the pole vector.
	// The pole vector defines a point that the middle joint should "aim towards",
	// controlling the bending plane (e.g., elbow bends forward vs backward).
	pole_translator = WidgetManipulatorTranslator::create();
	pole_translator->setTransform(translate(Vec3(anim_script->getParamVec3("ik_pole"))));
	pole_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(pole_translator);

	// Enable debug visualization
	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesIKSample::update()
{
	if (anim_script.isNull())
		return;

	PlayerPtr player = Game::getPlayer();
	if (player.isValid() == false)
		return;

	// Sync manipulators with the current camera
	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	pole_translator->setModelview(player->getCamera()->getModelview());
	pole_translator->setProjection(player->getCamera()->getProjection());

	// Update IK solver with current manipulator positions
	Vec3 target_pos = target_translator->getTransform().getTranslate();
	Vec3 pole_pos = pole_translator->getTransform().getTranslate();

	anim_script->setParamVec3("ik_target", vec3(target_pos));
	anim_script->setParamVec3("ik_pole", vec3(pole_pos));

	Visualizer::renderMessage3D(target_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me",
		vec4_white, 1, 30);
	Visualizer::renderMessage3D(pole_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me", vec4_white,
		1, 30);

	Visualizer::renderSolidSphere(0.02f, translate(target_pos), vec4_green);
	Visualizer::renderSolidSphere(0.02f, translate(pole_pos), vec4_red);

	if (0 < skeleton_pose->getNumLayers())
	{
		skeleton_pose->renderLayerBones(0, skeleton_pose->getWorldTransform(), vec4_black);

		if (root_joint != -1)
		{
			Vec3 root_joint_pos(skeleton_pose->getLayerJointObjectTransform(0, root_joint).getTranslate());
			root_joint_pos = skeleton_pose->getWorldTransform() * root_joint_pos;

			Visualizer::renderLine3D(root_joint_pos, target_pos, pole_pos, root_joint_pos, vec4_red);
			Visualizer::renderTriangle3D(root_joint_pos, target_pos, pole_pos, vec4(1.0f, 0.0f, 0.0f, 0.2f));
		}
	}
}

void AnimationBonesIKSample::shutdown()
{
	// Restore default visualizer settings
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
