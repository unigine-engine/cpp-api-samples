#include "AnimationBonesConstraintsSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesConstraintsSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesConstraintsSample::init()
{
#ifndef DEBUG
	if (Engine::get()->getBuildConfiguration() == Engine::BUILD_CONFIG_RELEASE)
		Log::warning("Current build configuration is Release. Visualization of ObjectMeshSkinned "
					 "Bones is not available with this build configuration\n");
#endif

	skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesConstraintsSample::init(): skinned is null\n");
		return;
	}

	// create IK chain and initialize bone sequence
	chain_id = skinned->addIKChain();
	skinned->setIKChainConstraint(ObjectMeshSkinned::CHAIN_CONSTRAINT_BONE_ROTATIONS, chain_id);
	for (int i = 0; i < ik_chain_bones.size(); i++)
		skinned->addIKChainBone(ik_chain_bones[i], chain_id);

	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(2.0f, 2.0f, 0.5f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);

	// create constraints
	for (int i = 0; i < constraints.size(); i++)
	{
		const Constraint &c = constraints[i];
		int index = skinned->addBoneConstraint(c.name);
		if (index == -1)
			continue;

		skinned->setBoneConstraintYawAxis(c.yaw_axis, index);
		skinned->setBoneConstraintPitchAxis(c.pitch_axis, index);
		skinned->setBoneConstraintRollAxis(c.roll_axis, index);

		skinned->setBoneConstraintYawAngles(c.yaw_min_angle, c.yaw_max_angle, index);
		skinned->setBoneConstraintPitchAngles(c.pitch_min_angle, c.pitch_max_angle, index);
		skinned->setBoneConstraintRollAngles(c.roll_min_angle, c.roll_max_angle, index);

		skinned->addVisualizeConstraint(index);
	}

	skinned->addVisualizeIKChain(chain_id);
	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesConstraintsSample::update()
{
	if (chain_id == -1)
		return;

	PlayerPtr player = Game::getPlayer();
	if (player.isValid() == false)
		return;

	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
}

void AnimationBonesConstraintsSample::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
