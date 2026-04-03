#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include "BonesSandbox.h"

// Interactive sandbox for experimenting with skeletal animation features.
// Provides UI editors for configuring IK chains, LookAt chains, and bone constraints
// in real-time, allowing visualization and testing of all available settings.
class AnimationBonesSandboxSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesSandboxSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to ObjectMeshSkinned node containing the skeletal mesh
	PROP_PARAM(Node, mesh_skinned_node);

private:
	void init();
	void shutdown();

private:
	Unigine::ObjectMeshSkinnedLegacyPtr skinned;

	// Separate window containing all editor panels
	Unigine::EngineWindowViewportPtr editors_window;

	Unigine::WidgetHBoxPtr main_hbox;
	Unigine::WidgetVBoxPtr main_vbox;

	// UI components for different animation features
	BoneSelection bone_selection;       // Bone hierarchy tree for selecting bones
	LookAtEditor look_at_editor;        // Editor for LookAt chain configuration
	IKEditor ik_editor;                 // Editor for IK chain configuration
	ConstraintEditor constraint_editor; // Editor for bone rotation constraints
};
