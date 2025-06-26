#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesMasksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesMasksSample, ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_ARRAY(String, rotation_only_bones);

private:
	void init();

private:
};
