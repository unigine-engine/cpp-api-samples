#include "AnimationBonesSimpleFootPlacementSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesSimpleFootPlacementSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesSimpleFootPlacementSample::init()
{
	skeleton_pose = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node.get());
	if (skeleton_pose.isValid() == false)
	{
		Log::error("AnimationBonesSimpleFootPlacementSample::init(): skeleton pose is null\n");
		return;
	}

	anim_script = skeleton_pose->getAnimScript();
	if (anim_script.isValid() == false)
	{
		Log::error("AnimationBonesSimpleFootPlacementSample::init(): animation script is null\n");
		return;
	}

	ConstSkeletonPtr skeleton = skeleton_pose->getSkeleton();
	if (skeleton.isValid() == false)
	{
		Log::error("AnimationBonesSimpleFootPlacementSample::init(): skeleton is null\n");
		return;
	}

	effector_joints[FOOT_LEFT] = skeleton->findJoint(left_foot_name.get());
	effector_joints[FOOT_RIGHT] = skeleton->findJoint(right_foot_name.get());

	src_effector_rotations[FOOT_LEFT] = anim_script->getParamQuat("left_foot_rotation");
	src_effector_rotations[FOOT_RIGHT] = anim_script->getParamQuat("right_foot_rotation");

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

	// Performs raycasting to find the ground under each foot and updates IK targets.
	if (skeleton_pose->getNumLayers() == 0)
		return;

	const Mat4 world_transform = skeleton_pose->getWorldTransform();
	const Mat4 world_itransform = skeleton_pose->getIWorldTransform();

	const quat world_rotation = world_transform.getRotate();
	const quat world_irotation = world_itransform.getRotate();

	for (int i = FOOT_LEFT; i < FOOT_NUM; i++)
	{
		if (effector_joints[i] == -1)
			continue;

		// Get current foot position and rotation from animation
		Mat4 t = world_transform * Mat4(skeleton_pose->getLayerJointObjectTransform(0, effector_joints[i]));
		Vec3 foot_pos(t.getTranslate());
		quat foot_rot(t.getRotate());
		bool ik_enabled = false;

		// Visualize the raycast line
		Visualizer::renderVector(foot_pos + Vec3_up * 0.5f, foot_pos + Vec3_down * foot_height, vec4_blue);

		// Cast a ray downward from the foot to find ground contact
		PhysicsIntersectionNormalPtr intersection = PhysicsIntersectionNormal::create();
		ObjectPtr obj = Physics::getIntersection(foot_pos + Vec3_up * 0.5f, foot_pos + Vec3_down * foot_height * 1.1f, 1, intersection);
		if (obj != nullptr)
		{
			// Visualize surface normal at contact point
			Visualizer::renderVector(intersection->getPoint(), intersection->getPoint() + Vec3(intersection->getNormal()), vec4_blue);

			// Adjust foot position to match ground height (plus foot_height offset)
			foot_pos = intersection->getPoint() + Vec3_up * foot_height;
			// Rotate foot to align with surface normal
			foot_rot = rotationFromTo(vec3_up, intersection->getNormal()) * world_rotation * src_effector_rotations[i];

			if (intersection->getShape().isValid())
				ik_enabled = true;
		}

		// Update IK target with adjusted position and rotation
		if (i == FOOT_LEFT)
		{
			float w = anim_script->getParamFloat("left_effector_weight");
			if (ik_enabled)
				w = lerp(w, 1.0f, 1.0f - exp(-Game::getIFps() * 20.0f));
			else
				w = lerp(w, 0.0f, 1.0f - exp(-Game::getIFps() * 5.0f));
			anim_script->setParamFloat("left_effector_weight", saturate(w));

			anim_script->setParamVec3("left_foot_position", vec3(world_itransform * foot_pos));
			anim_script->setParamQuat("left_foot_rotation", world_irotation * foot_rot);
		} else if (i == FOOT_RIGHT)
		{
			float w = anim_script->getParamFloat("right_effector_weight");
			if (ik_enabled)
				w = lerp(w, 1.0f, 1.0f - exp(-Game::getIFps() * 20.0f));
			else
				w = lerp(w, 0.0f, 1.0f - exp(-Game::getIFps() * 5.0f));
			anim_script->setParamFloat("right_effector_weight", saturate(w));

			anim_script->setParamVec3("right_foot_position", vec3(world_itransform * foot_pos));
			anim_script->setParamQuat("right_foot_rotation", world_irotation * foot_rot);
		}
	}
}

void AnimationBonesSimpleFootPlacementSample::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
