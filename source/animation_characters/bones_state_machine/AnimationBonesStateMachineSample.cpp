#include "AnimationBonesStateMachineSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(AnimationBonesStateMachineSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesStateMachineSample::init()
{
	// Walk state machine
	pose_walk = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node_walk.get());
	if (pose_walk.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): pose walk is null\n");
		return;
	}

	walk_script = pose_walk->getAnimScript();
	if (walk_script.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): walk script is null\n");
		return;
	}

	// Idle turn state machine
	pose_idle_turn = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node_idle_turn.get());
	if (pose_idle_turn.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): pose_idle_turn is null\n");
		return;
	}

	idle_turn_script = pose_idle_turn->getAnimScript();
	if (idle_turn_script.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): idle turn script is null\n");
		return;
	}

	// Walk run state machine
	pose_walk_run = checked_ptr_cast<NodeSkeletonPose>(skeleton_pose_node_walk_run.get());
	if (pose_walk_run.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): pose walk run is null\n");
		return;
	}

	walk_run_script = pose_walk_run->getAnimScript();
	if (walk_run_script.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): walk run script is null\n");
		return;
	}
}

void AnimationBonesStateMachineSample::update()
{
	// Walk state machine: demonstrates speed-based state transitions
	// The "speed" variable controls idle<->walk transitions
	// The "turn" flag triggers a 180-degree turn-around animation
	{
		if (Input::isKeyDown(Input::KEY_T))
			walk_script->setParamFloat("speed", 1.0f);

		if (Input::isKeyDown(Input::KEY_G))
			walk_script->setParamFloat("speed", 0.0f);

		walk_script->setParamBool("turn", false);
		if (Input::isKeyPressed(Input::KEY_Y))
			walk_script->setParamBool("turn", true);

		// Apply root motion: the animation's root bone movement is extracted
		// and applied to the character's world transform for realistic locomotion
		if (walk_script->isActiveRootMotion())
		{
			Transform delta = walk_script->getRootMotionDelta();
			pose_walk->setWorldTransform(pose_walk->getWorldTransform() * Mat4(delta.getMat()));
		}
	}

	// Idle turn state machine: demonstrates 1D blend space
	// The "turn" variable (-1 to 1) blends between turn_left, idle, and turn_right animations
	// This creates smooth directional turning while standing still
	{
		float turn = idle_turn_script->getParamFloat("turn");
		if (Input::isKeyPressed(Input::KEY_V))
			turn += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_C))
			turn -= Game::getIFps();
		turn = clamp(turn, -1.0f, 1.0f);
		idle_turn_script->setParamFloat("turn", turn);

		if (idle_turn_script->isActiveRootMotion())
		{
			Transform delta = idle_turn_script->getRootMotionDelta();
			pose_idle_turn->setWorldTransform(pose_idle_turn->getWorldTransform() * Mat4(delta.getMat()));
		}
	}

	// Walk/run state machine: demonstrates 2D blend space
	// Two variables control blending: "y" (0-1) for forward/backward speed,
	// "x" (-1 to 1) for strafe direction. This creates 8-directional movement
	// by blending between walk_forward, walk_backward, strafe_left, strafe_right, etc.
	{
		float y = walk_run_script->getParamFloat("y");
		if (Input::isKeyPressed(Input::KEY_I))
			y += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_K))
			y -= Game::getIFps();
		y = saturate(y);
		walk_run_script->setParamFloat("y", y);

		float x = walk_run_script->getParamFloat("x");
		if (Input::isKeyPressed(Input::KEY_L))
			x += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_J))
			x -= Game::getIFps();
		x = clamp(x, -1.0f, 1.0f);
		walk_run_script->setParamFloat("x", x);

		if (walk_run_script->isActiveRootMotion())
		{
			Transform delta = walk_run_script->getRootMotionDelta();
			pose_walk_run->setWorldTransform(pose_walk_run->getWorldTransform() * Mat4(delta.getMat()));
		}
	}
}
