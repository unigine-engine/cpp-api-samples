#include "AnimationBonesStateMachineSample.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>
#include <UnigineProperties.h>

REGISTER_COMPONENT(AnimationBonesStateMachineSample);

using namespace Unigine;
using namespace Math;

void AnimationBonesStateMachineSample::init()
{
	// walk state machine
	skinned_machine_walk = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node_machine_walk.get());
	if (skinned_machine_walk.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): skinned is null\n");
		return;
	}

	UGUID prop_guid = FileSystem::pathToGuid(state_machine_walk_prop_path.get());
	PropertyPtr state_machine_walk_prop = Properties::findPropertyByGUID(prop_guid);
	if (state_machine_walk_prop.isNull())
		state_machine_walk_prop = Properties::findPropertyByPath(state_machine_walk_prop_path.get());

	state_machine_walk.initFromProperty(skinned_machine_walk, state_machine_walk_prop, Math::AXIS(axis_forward.get()));

	// idle turn state machine
	skinned_machine_idle_turn = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node_machine_idle_turn.get());
	if (skinned_machine_idle_turn.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): skinned is null\n");
		return;
	}

	prop_guid = FileSystem::pathToGuid(state_machine_idle_turn_prop_path.get());
	PropertyPtr state_machine_idle_turn_prop = Properties::findPropertyByGUID(prop_guid);
	if (state_machine_idle_turn_prop.isNull())
		state_machine_idle_turn_prop = Properties::findPropertyByPath(state_machine_idle_turn_prop_path.get());

	state_machine_idle_turn.initFromProperty(skinned_machine_idle_turn, state_machine_idle_turn_prop, Math::AXIS(axis_forward.get()));

	// walk run state machine
	skinned_machine_walk_run = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node_machine_walk_run.get());
	if (skinned_machine_walk_run.isValid() == false)
	{
		Log::error("AnimationBonesStateMachineSample::init(): skinned is null\n");
		return;
	}

	prop_guid = FileSystem::pathToGuid(state_machine_walk_run_prop_path.get());
	PropertyPtr state_machine_walk_run_prop = Properties::findPropertyByGUID(prop_guid);
	if (state_machine_walk_run_prop.isNull())
		state_machine_walk_run_prop = Properties::findPropertyByPath(state_machine_walk_run_prop_path.get());

	state_machine_walk_run.initFromProperty(skinned_machine_walk_run, state_machine_walk_run_prop, Math::AXIS(axis_forward.get()));
}

void AnimationBonesStateMachineSample::update()
{
	// walk
	{
		if (Input::isKeyDown(Input::KEY_T))
			state_machine_walk.setFloatVariable("speed", 1.0f);

		if (Input::isKeyDown(Input::KEY_G))
			state_machine_walk.setFloatVariable("speed", 0.0f);

		state_machine_walk.setFlagVariable("turn", 0);
		if (Input::isKeyDown(Input::KEY_Y))
			state_machine_walk.setFlagVariable("turn", 1);

		skinned_machine_walk->setWorldTransform(skinned_machine_walk->getWorldTransform() * Mat4(state_machine_walk.getRootMotionDelta()));
	}

	// idle turn
	{
		float turn = state_machine_idle_turn.getFloatVariable("turn");
		if (Input::isKeyPressed(Input::KEY_V))
			turn += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_C))
			turn -= Game::getIFps();
		turn = clamp(turn, -1.0f, 1.0f);
		state_machine_idle_turn.setFloatVariable("turn", turn);

		skinned_machine_idle_turn->setWorldTransform(skinned_machine_idle_turn->getWorldTransform() * Mat4(state_machine_idle_turn.getRootMotionDelta()));
	}

	// walk run
	{
		float y = state_machine_walk_run.getFloatVariable("y");
		if (Input::isKeyPressed(Input::KEY_I))
			y += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_K))
			y -= Game::getIFps();
		y = saturate(y);
		state_machine_walk_run.setFloatVariable("y", y);

		float x = state_machine_walk_run.getFloatVariable("x");
		if (Input::isKeyPressed(Input::KEY_L))
			x += Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_J))
			x -= Game::getIFps();
		x = clamp(x, -1.0f, 1.0f);
		state_machine_walk_run.setFloatVariable("x", x);

		skinned_machine_walk_run->setWorldTransform(skinned_machine_walk_run->getWorldTransform() * Mat4(state_machine_walk_run.getRootMotionDelta()));
	}
}
