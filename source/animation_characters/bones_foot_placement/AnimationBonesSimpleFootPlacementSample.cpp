#include "AnimationBonesSimpleFootPlacementSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesSimpleFootPlacementSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesSimpleFootPlacementSample::init()
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
		Log::error("AnimationBonesSimpleFootPlacementSample::init(): skinned is null\n");
		return;
	}

	// Create IK chain for left leg
	chain_ids[FOOT_LEFT] = skinned->addIKChain();
	for (int i = 0; i < left_leg_bones.size(); i++)
	{
		int bone = skinned->findBone(left_leg_bones[i]);
		if (bone == -1)
			continue;

		skinned->addIKChainBone(bone, chain_ids[FOOT_LEFT]);
		effector_bones[FOOT_LEFT] = bone; // Last added bone becomes the effector
	}
	skinned->setIKChainPoleWorldPosition(Vec3(0.5, 1.0f, 1.5), chain_ids[FOOT_LEFT]);

	// Create IK chain for right leg
	chain_ids[FOOT_RIGHT] = skinned->addIKChain();
	for (int i = 0; i < right_leg_bones.size(); i++)
	{
		int bone = skinned->findBone(right_leg_bones[i]);
		if (bone == -1)
			continue;

		skinned->addIKChainBone(bone, chain_ids[FOOT_RIGHT]);
		effector_bones[FOOT_RIGHT] = bone;
	}
	skinned->setIKChainPoleWorldPosition(Vec3(1.0, 0.5f, 1.5), chain_ids[FOOT_RIGHT]);

	// Initialize IK targets at current foot positions and enable effector rotation
	// (so feet align with the surface normal, not just position)
	for (int i = FOOT_LEFT; i < FOOT_NUM; i++)
	{
		if (effector_bones[i] == -1)
			continue;

		Mat4 t = skinned->getBoneWorldTransform(effector_bones[i]);
		skinned->setIKChainTargetWorldPosition(t.getTranslate(), chain_ids[i]);

		skinned->setIKChainUseEffectorRotation(true, chain_ids[i]);
		skinned->setIKChainEffectorWorldRotation(t.getRotate(), chain_ids[i]);

		skinned->addVisualizeIKChain(chain_ids[i]);
	}

	// Create manipulators for moving/rotating the obstacle platform
	if (obstacle.get().isValid())
	{
		translator = WidgetManipulatorTranslator::create();
		translator->setLifetime(Widget::LIFETIME_WORLD);
		translator->setTransform(obstacle->getWorldTransform());
		WindowManager::getMainWindow()->addChild(translator);

		rotator = WidgetManipulatorRotator::create();
		rotator->setLifetime(Widget::LIFETIME_WORLD);
		rotator->setTransform(obstacle->getWorldTransform());
		rotator->setHidden(true);
		WindowManager::getMainWindow()->addChild(rotator);
	}

	// Subscribe to the event that fires just before IK solving.
	// This is where foot positions are updated based on raycasts.
	skinned->getEventBeginIKSolvers().connect(this, &AnimationBonesSimpleFootPlacementSample::on_begin_ik_solvers);

	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesSimpleFootPlacementSample::update()
{
	PlayerPtr player = Game::getPlayer();
	if (obstacle.get().isValid() && player.isValid())
	{
		// Toggle between translation (T) and rotation (R) manipulators
		if (Input::isKeyDown(Input::KEY_T))
		{
			translator->setHidden(false);
			rotator->setHidden(true);
			translator->setTransform(obstacle->getWorldTransform());
		}

		if (Input::isKeyDown(Input::KEY_R))
		{
			rotator->setHidden(false);
			translator->setHidden(true);
			rotator->setTransform(obstacle->getWorldTransform());
		}

		// Apply manipulator changes to the obstacle
		if (translator->isHidden() == false)
		{
			translator->setModelview(player->getCamera()->getModelview());
			translator->setProjection(player->getCamera()->getProjection());
			obstacle->setWorldTransform(translator->getTransform());
		}

		if (rotator->isHidden() == false)
		{
			rotator->setModelview(player->getCamera()->getModelview());
			rotator->setProjection(player->getCamera()->getProjection());
			obstacle->setWorldTransform(rotator->getTransform());
		}

		Visualizer::renderMessage3D(translator->getTransform().getTranslate()
				+ Vec3(0.f, 0.f, -0.1f),
			vec3_zero, "Drag Me", vec4_white, 1, 30);
	}
}

void AnimationBonesSimpleFootPlacementSample::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}

// This callback is invoked just before IK solvers run.
// It performs raycasting to find the ground under each foot and updates IK targets.
void AnimationBonesSimpleFootPlacementSample::on_begin_ik_solvers()
{
	for (int i = FOOT_LEFT; i < FOOT_NUM; i++)
	{
		if (effector_bones[i] == -1)
			continue;

		// Get current foot position and rotation from animation
		Mat4 t = skinned->getBoneWorldTransform(effector_bones[i]);
		Vec3 foot_pos(t.getTranslate());
		quat foot_rot(t.getRotate());

		// Visualize the raycast line
		Visualizer::renderVector(foot_pos + Vec3_up * 0.5f, foot_pos + Vec3_down * foot_height, vec4_blue);

		// Cast a ray downward from the foot to find ground contact
		PhysicsIntersectionNormalPtr intersection = PhysicsIntersectionNormal::create();
		ObjectPtr obj = Physics::getIntersection(foot_pos + Vec3_up * 0.5f, foot_pos + Vec3_down * foot_height, 1, intersection);
		if (obj != nullptr)
		{
			// Visualize surface normal at contact point
			Visualizer::renderVector(intersection->getPoint(), intersection->getPoint() + Vec3(intersection->getNormal()), vec4_blue);

			// Adjust foot position to match ground height (plus foot_height offset)
			foot_pos = intersection->getPoint() + Vec3_up * foot_height;
			// Rotate foot to align with surface normal
			foot_rot = rotationFromTo(vec3_up, intersection->getNormal()) * foot_rot;
		}

		// Update IK target with adjusted position and rotation
		skinned->setIKChainTargetWorldPosition(foot_pos, chain_ids[i]);
		skinned->setIKChainEffectorWorldRotation(foot_rot, chain_ids[i]);
	}
}
