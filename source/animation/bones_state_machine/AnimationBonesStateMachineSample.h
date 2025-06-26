#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include "StateMachine.h"

class AnimationBonesStateMachineSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesStateMachineSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Switch, axis_forward, 2, "X,Y,Z,NX,NY,NZ")

	PROP_PARAM(Node, mesh_skinned_node_machine_walk);
	PROP_PARAM(File, state_machine_walk_prop_path);

	PROP_PARAM(Node, mesh_skinned_node_machine_idle_turn);
	PROP_PARAM(File, state_machine_idle_turn_prop_path);

	PROP_PARAM(Node, mesh_skinned_node_machine_walk_run);
	PROP_PARAM(File, state_machine_walk_run_prop_path);

private:
	void init();
	void update();

private:
	Unigine::ObjectMeshSkinnedPtr skinned_machine_walk;
	AnimationSystem::StateMachine state_machine_walk;

	Unigine::ObjectMeshSkinnedPtr skinned_machine_idle_turn;
	AnimationSystem::StateMachine state_machine_idle_turn;

	Unigine::ObjectMeshSkinnedPtr skinned_machine_walk_run;
	AnimationSystem::StateMachine state_machine_walk_run;
};
