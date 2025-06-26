#include "AnimationBonesIKSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesIKSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesIKSample::init()
{
#ifndef DEBUG
	if (Engine::get()->getBuildConfiguration() == Engine::BUILD_CONFIG_RELEASE)
		Log::warning("Current build configuration is Release. Visualization of ObjectMeshSkinned "
					 "Bones is not available with this build configuration\n");
#endif

	skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesIKSample::init(): skinned is null\n");
		return;
	}

	// create IK chain and initialize bone sequence
	chain_id = skinned->addIKChain();
	for (int i = 0; i < bones.size(); i++)
		skinned->addIKChainBone(bones[i], chain_id);

	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(-0.175f, 0.5f, 0.5f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	pole_translator = WidgetManipulatorTranslator::create();
	pole_translator->setTransform(translate(Vec3(-0.3f, 1.0f, 1.0f)));
	pole_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(pole_translator);

	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setIKChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);

	skinned->addVisualizeIKChain(chain_id);
	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesIKSample::update()
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

	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setIKChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);
}

void AnimationBonesIKSample::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
