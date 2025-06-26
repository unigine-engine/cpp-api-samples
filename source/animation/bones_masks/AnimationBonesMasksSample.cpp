#include "AnimationBonesMasksSample.h"

#include <UnigineGui.h>

REGISTER_COMPONENT(AnimationBonesMasksSample);

using namespace Unigine;

void AnimationBonesMasksSample::init()
{
	ObjectMeshSkinnedPtr skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesMasksSample::init(): skinned is null\n");
		return;
	}

	skinned->setLayerFrameUsesEnabled(0, true);
	for (int i = 0; i < rotation_only_bones.size(); i++)
	{
		int bone = skinned->findBone(rotation_only_bones[i]);
		if (bone != -1)
			skinned->setLayerBoneFrameUses(0, bone, ObjectMeshSkinned::ANIM_FRAME_USES_ROTATION);
	}
}
