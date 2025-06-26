#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include "BonesSandbox.h"

class AnimationBonesSandboxSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesSandboxSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);

private:
	void init();
	void shutdown();

private:
	Unigine::ObjectMeshSkinnedPtr skinned;

	Unigine::EngineWindowViewportPtr editors_window;

	Unigine::WidgetHBoxPtr main_hbox;
	Unigine::WidgetVBoxPtr main_vbox;

	BoneSelection bone_selection;
	LookAtEditor look_at_editor;
	IKEditor ik_editor;
	ConstraintEditor constraint_editor;
};
