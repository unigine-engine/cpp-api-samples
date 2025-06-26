#include "AnimationBonesRetargetingSample.h"

#include <UnigineGui.h>

REGISTER_COMPONENT(AnimationBonesRetargetingSample);

using namespace Unigine;

void AnimationBonesRetargetingSample::init()
{
	ObjectMeshSkinnedPtr skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesRetargetingSample::init(): skinned is null\n");
		return;
	}

	BonesRetargetingPtr retargeting = BonesRetargeting::create();

	// Loading the names of bones from two skeletons
	// to further configure the mapping between them.
	bool res = retargeting->loadBones(src_mesh_path, skinned->getMeshPath());
	if (res == false)
	{
		Log::error("AnimationBonesRetargetingSample::init(): can't load bones from meshes\n");
		return;
	}

	// In our example the skeletons have the same bone names,
	// so we will use a special method to calculate the proportions.
	// In general, you can use setNameMapping(src_bone, dst_bone).
	retargeting->findEqualNameMapping();

	// By default, all bones use positions from their default position. (T-pose and MODE_BIND).
	// But in order to avoid the character "hanging" in the air, we must adjust the bones
	// standing on the ground and the hip bones. To do this, we transfer them to the proportion mode.
	if (String::isEmpty(on_ground_bone_name) == false)
		retargeting->setBoneMode(on_ground_bone_name, BonesRetargeting::MODE_PROPORTION);

	if (String::isEmpty(hips_bone_name) == false)
		retargeting->setBoneMode(hips_bone_name, BonesRetargeting::MODE_PROPORTION);

	// Register retargeting for a specific mesh and a specific animation.
	ObjectMeshSkinned::setRetargeting(retargeting, anim_path, skinned->getMeshPath());

	// And update the animation on the default zero layer.
	skinned->setAnimPath(anim_path);
}

void AnimationBonesRetargetingSample::shutdown()
{
	ObjectMeshSkinnedPtr skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid())
		ObjectMeshSkinned::removeRetargeting(anim_path, skinned->getMeshPath());
}
