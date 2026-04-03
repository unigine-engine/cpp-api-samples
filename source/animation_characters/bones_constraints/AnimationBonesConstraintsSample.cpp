#include "AnimationBonesConstraintsSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesConstraintsSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesConstraintsSample::init()
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
		Log::error("AnimationBonesConstraintsSample::init(): skinned is null\n");
		return;
	}

	// Create IK chain and add bones to it.
	// CHAIN_CONSTRAINT_BONE_ROTATIONS enables per-bone rotation limits during IK solving.
	chain_id = skinned->addIKChain();
	skinned->setIKChainConstraint(ObjectMeshSkinnedLegacy::CHAIN_CONSTRAINT_BONE_ROTATIONS, chain_id);
	for (int i = 0; i < ik_chain_bones.size(); i++)
		skinned->addIKChainBone(ik_chain_bones[i], chain_id);

	// Create a 3D manipulator widget for interactive IK target positioning
	target_translator = WidgetManipulatorTranslator::create();
	target_translator->setTransform(translate(Vec3(2.0f, 2.0f, 0.5f)));
	target_translator->setLifetime(Widget::LIFETIME_WORLD);
	WindowManager::getMainWindow()->addChild(target_translator);

	skinned->setIKChainTargetWorldPosition(target_translator->getTransform().getTranslate(), chain_id);

	// Apply rotation constraints to each bone specified in the constraints array.
	// Constraints limit how far a bone can rotate along its local axes.
	for (int i = 0; i < constraints.size(); i++)
	{
		const Constraint &c = constraints[i];
		int index = skinned->addBoneConstraint(c.name);
		if (index == -1)
			continue;

		// Define local axes for yaw/pitch/roll rotations
		skinned->setBoneConstraintYawAxis(c.yaw_axis, index);
		skinned->setBoneConstraintPitchAxis(c.pitch_axis, index);
		skinned->setBoneConstraintRollAxis(c.roll_axis, index);

		// Set min/max angle limits for each rotation axis
		skinned->setBoneConstraintYawAngles(c.yaw_min_angle, c.yaw_max_angle, index);
		skinned->setBoneConstraintPitchAngles(c.pitch_min_angle, c.pitch_max_angle, index);
		skinned->setBoneConstraintRollAngles(c.roll_min_angle, c.roll_max_angle, index);

		// Enable debug visualization for this constraint (shows allowed rotation cone)
		skinned->addVisualizeConstraint(index);
	}

	// Enable debug visualization for the IK chain (shows bones and joints)
	skinned->addVisualizeIKChain(chain_id);
	Visualizer::setEnabled(true);
	// Disable depth test so visualizations are always visible (even through geometry)
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesConstraintsSample::update()
{
	if (chain_id == -1)
		return;

	PlayerPtr player = Game::getPlayer();
	if (player.isValid() == false)
		return;

	// Sync the manipulator widget with the current camera so it renders correctly
	target_translator->setModelview(player->getCamera()->getModelview());
	target_translator->setProjection(player->getCamera()->getProjection());

	// Update IK target position based on current manipulator position
	Vec3 pos = target_translator->getTransform().getTranslate();
	skinned->setIKChainTargetWorldPosition(pos, chain_id);
	Visualizer::renderMessage3D(pos + Vec3(0.f,0.f,-0.1f), vec3_zero, "Drag Me", vec4_white, 1, 30);
}

void AnimationBonesConstraintsSample::shutdown()
{
	// Restore default visualizer settings
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
