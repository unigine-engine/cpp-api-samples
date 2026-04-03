#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates bone masks for selective animation data application.
// Masks control which transformation components (position, rotation, scale) are
// applied to specific bones, enabling animation reuse across differently-sized characters.
class AnimationBonesMasksSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesMasksSample, ComponentBase);
	COMPONENT_INIT(init);

	// Reference to ObjectMeshSkinned node containing the skeletal mesh
	PROP_PARAM(Node, mesh_skinned_node);
	// Bones that will only receive rotation data (position and scale are ignored).
	// Useful for retargeting animations from differently-proportioned characters.
	PROP_ARRAY(String, rotation_only_bones);

private:
	void init();
};
