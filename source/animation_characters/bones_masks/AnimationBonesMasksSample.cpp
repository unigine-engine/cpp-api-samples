#include "AnimationBonesMasksSample.h"

#include <UnigineGui.h>

REGISTER_COMPONENT(AnimationBonesMasksSample);

using namespace Unigine;

void AnimationBonesMasksSample::init()
{
	ObjectMeshSkinnedLegacyPtr skinned = checked_ptr_cast<ObjectMeshSkinnedLegacy>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesMasksSample::init(): skinned is null\n");
		return;
	}

	// Enable per-bone frame uses (masks) on layer 0.
	// This allows specifying which transform components each bone should receive.
	skinned->setLayerFrameUsesEnabled(0, true);

	// Apply rotation-only mask to specified bones.
	// ANIM_FRAME_USES_ROTATION means only rotation data is taken from the animation,
	// while position and scale remain unchanged (preserving original bone proportions).
	for (int i = 0; i < rotation_only_bones.size(); i++)
	{
		int bone = skinned->findBone(rotation_only_bones[i]);
		if (bone != -1)
			skinned->setLayerBoneFrameUses(0, bone, ObjectMeshSkinnedLegacy::ANIM_FRAME_USES_ROTATION);
	}
}
