#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesRetargetingSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesRetargetingSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_PARAM(File, src_mesh_path, "");
	PROP_PARAM(File, anim_path, "");
	PROP_PARAM(String, on_ground_bone_name, "");
	PROP_PARAM(String, hips_bone_name, "");

private:
	void init();
	void shutdown();
};
