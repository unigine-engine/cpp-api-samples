#include "AnimationBonesIKSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesIKSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesIKSample::init()
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
		Log::error("AnimationBonesIKSample::init(): skinned is null\n");
		return;
	}

	// Create IK chain and add bones to it
	chain_id = skinned->addIKChain();
	for (int i = 0; i < bones.size(); i++)
		skinned->addIKChainBone(bones[i], chain_id);

	// Create manipulator for the IK target (end effector goal position)
	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(-0.175f, 0.5f, 0.5f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	// Create manipulator for the pole vector.
	// The pole vector defines a point that the middle joint should "aim towards",
	// controlling the bending plane (e.g., elbow bends forward vs backward).
	pole_translator = WidgetManipulatorTranslator::create();
	pole_translator->setTransform(translate(Vec3(-0.3f, 1.0f, 1.0f)));
	pole_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(pole_translator);

	// Set initial IK target and pole positions
	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);
	skinned->setIKChainPoleWorldPosition(pole_translator->getTransform().getTranslate(), chain_id);

	// Enable debug visualization for the IK chain
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

	// Sync manipulators with the current camera
	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	pole_translator->setModelview(player->getCamera()->getModelview());
	pole_translator->setProjection(player->getCamera()->getProjection());

	// Update IK solver with current manipulator positions
	Vec3 target_pos = target_translator->getTransform().getTranslate();
	Vec3 pole_pos = pole_translator->getTransform().getTranslate();

	skinned->setIKChainTargetWorldPosition(target_pos, chain_id);
	skinned->setIKChainPoleWorldPosition(pole_pos, chain_id);

	Visualizer::renderMessage3D(target_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me",
		vec4_white, 1, 30);
	Visualizer::renderMessage3D(pole_pos + Vec3(0.f, 0.f, -0.1f), vec3_zero, "Drag Me", vec4_white,
		1, 30);
}

void AnimationBonesIKSample::shutdown()
{
	// Restore default visualizer settings
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
