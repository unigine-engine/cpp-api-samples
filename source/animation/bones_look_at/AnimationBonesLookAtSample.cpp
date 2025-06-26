#include "AnimationBonesLookAtSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesLookAtSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesLookAtSample::init()
{
#ifndef DEBUG
	if (Engine::get()->getBuildConfiguration() == Engine::BUILD_CONFIG_RELEASE)
		Log::warning("Current build configuration is Release. Visualization of ObjectMeshSkinned "
					 "Bones is not available with this build configuration\n");
#endif

	skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesLookAtSample::init(): skinned is null\n");
		return;
	}

	// create LookAt chain and initialize bone sequence
	chain_id = skinned->addLookAtChain();
	for (int i = 0; i < bones.size(); i++)
	{
		const Bone &b = bones[i];
		int index = skinned->addLookAtChainBone(b.name, chain_id);
		if (index == -1)
			continue;

		// these axes must be in the local basis of the bone
		skinned->setLookAtChainBoneAxis(Vec3(b.axis), index, chain_id);
		skinned->setLookAtChainBoneUp(Vec3(b.up), index, chain_id);

		skinned->setLookAtChainBoneWeight(b.weight, index, chain_id);
	}

	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(1.5f, 1.5f, 1.6f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	pole_translator = WidgetManipulatorTranslator::create();
	pole_translator->setTransform(translate(Vec3(0.0f, 0.0f, 3.0f)));
	pole_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(pole_translator);

	skinned->setLookAtChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setLookAtChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);

	skinned->addVisualizeLookAtChain(chain_id);
	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesLookAtSample::update()
{
	if (chain_id == -1)
		return;

	PlayerPtr player = Game::getPlayer();
	if (player.isValid() == false)
		return;

	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	pole_translator->setModelview(player->getCamera()->getModelview());
	pole_translator->setProjection(player->getCamera()->getProjection());

	skinned->setLookAtChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setLookAtChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);
}

void AnimationBonesLookAtSample::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
