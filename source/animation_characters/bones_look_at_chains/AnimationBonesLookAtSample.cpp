#include "AnimationBonesLookAtSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesLookAtSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesLookAtSample::init()
{
	// Bone visualization only works in Debug/Development builds
#ifndef DEBUG
	if (Engine::get()->getBuildConfiguration() == Engine::BUILD_CONFIG_RELEASE)
		Log::warning("Current build configuration is Release. Visualization of ObjectMeshSkinned "
					 "Bones is not available with this build configuration\n");
#endif

	skinned = checked_ptr_cast<ObjectMeshSkinnedLegacy>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesLookAtSample::init(): skinned is null\n");
		return;
	}

	// Create LookAt chain and configure each participating bone
	chain_id = skinned->addLookAtChain();
	for (int i = 0; i < bones.size(); i++)
	{
		const Bone &b = bones[i];
		int index = skinned->addLookAtChainBone(b.name, chain_id);
		if (index == -1)
			continue;

		// Axis and up vectors must be specified in the bone's local coordinate system
		skinned->setLookAtChainBoneAxis(Vec3(b.axis), index, chain_id);
		skinned->setLookAtChainBoneUp(Vec3(b.up), index, chain_id);

		// Weight determines how much this bone contributes to the overall look-at rotation
		skinned->setLookAtChainBoneWeight(b.weight, index, chain_id);
	}

	// Create manipulator for the look-at target
	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(1.5f, 1.5f, 1.6f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	// Create manipulator for the pole vector (defines the "up" reference for orientation)
	pole_translator = WidgetManipulatorTranslator::create();
	pole_translator->setTransform(translate(Vec3(0.0f, 0.0f, 3.0f)));
	pole_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(pole_translator);

	// Set initial target and pole positions
	skinned->setLookAtChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setLookAtChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);

	// Enable debug visualization for the LookAt chain
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

	// Sync manipulators with the current camera
	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	pole_translator->setModelview(player->getCamera()->getModelview());
	pole_translator->setProjection(player->getCamera()->getProjection());

	Vec3 target_pos = target_translator->getTransform().getTranslate();
	Vec3 pole_pos = pole_translator->getTransform().getTranslate();

	// Update LookAt chain with current manipulator positions
	skinned->setLookAtChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setLookAtChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);

	Visualizer::renderMessage3D(target_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me",
		vec4_white, 1, 30);
	Visualizer::renderMessage3D(pole_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me", vec4_white,
		1, 30);
}

void AnimationBonesLookAtSample::shutdown()
{
	// Restore default visualizer settings
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
