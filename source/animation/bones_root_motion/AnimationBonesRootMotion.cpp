#include "AnimationBonesRootMotion.h"

#include <UnigineGui.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(AnimationBonesRootMotion);

using namespace Unigine;
using namespace Math;

void FrameUpdater::init(ObjectMeshSkinnedPtr &skinned)
{
	// this callback can be called from any thread and not every frame
	// it depends on the object update settings
	// this ifps takes into account all these features
	skinned->getEventUpdate().connect(skinned_connection, [](float ifps, const ObjectMeshSkinnedPtr &obj)
	{
		// just update the frames in a loop
		float frame = obj->getLayerFrame(0);
		frame += ifps * 30.0f;

		const int num_frames = obj->getLayerNumFrames(0);
		if (frame >= (num_frames - 1))
			frame -= (num_frames - 1);

		obj->setLayerFrame(0, frame);
	});
}

void RootMotionFrameUpdater::init(ObjectMeshSkinnedPtr &skinned, const char *root_bone_name, Unigine::Math::AXIS in_axis_forward)
{
	axis_forward = in_axis_forward;

	// find the bone the offset of which will be applied to the object
	root_bone = skinned->findBone(root_bone_name);
	if (root_bone == -1)
		return;

	skinned->getEventUpdate().connect(skinned_connection, [this](float ifps, const ObjectMeshSkinnedPtr &obj)
	{
		float frame = obj->getLayerFrame(0);
		frame += ifps * 30.0f;

		const int num_frames = obj->getLayerNumFrames(0);
		if (frame >= (num_frames - 1))
		{
			// we need to save the bone offset to the last frame...
			obj->setLayerFrame(0, itof(num_frames - 1));
			mat4 bone_t = obj->getLayerBoneTransform(0, root_bone);
			update_proj_bone_transform(bone_t, axis_forward);
			last_frame_delta = inverse(prev_proj_bone_t) * proj_bone_t;

			// ...and update the previous transform to frame zero
			obj->setLayerFrame(0, 0.0f);
			bone_t = obj->getLayerBoneTransform(0, root_bone);
			update_proj_bone_transform(bone_t, axis_forward);
			prev_proj_bone_t = proj_bone_t;

			frame -= (num_frames - 1);
		} else
		{
			last_frame_delta = mat4_identity;
		}

		obj->setLayerFrame(0, frame);

		// get the bone transformation projected onto the XY plane
		mat4 bone_t = obj->getLayerBoneTransform(0, root_bone);
		update_proj_bone_transform(bone_t, axis_forward);

		// calculate the bone offset based on the previous transformation
		// and on the offset in the last frame
		root_motion_delta = inverse(prev_proj_bone_t) * (proj_bone_t * last_frame_delta);

		// save the last update frame, since this callback is not called every frame
		root_motion_delta_frame = Engine::get()->getFrame();

		// move the root bone to the origin
		bone_t = inverse(proj_bone_t) * bone_t;
		obj->setLayerBoneTransform(0, root_bone, bone_t);

		prev_proj_bone_t = proj_bone_t;
	});
}

void RootMotionFrameUpdater::applyRootMotion(ObjectMeshSkinnedPtr &skinned)
{
	// apply the offset to the object if it really was in this frame
	long long frame = Engine::get()->getFrame();
	if (frame == root_motion_delta_frame)
	{
		skinned->setWorldTransform(skinned->getWorldTransform() * Mat4(root_motion_delta));
		root_motion_delta = mat4_identity;
	}
}

void RootMotionFrameUpdater::update_proj_bone_transform(const mat4 &bone_t, AXIS axis_forward)
{
	vec3 forward_axis;
	switch (axis_forward) {
		case AXIS_X: forward_axis = bone_t.getAxisX(); break;
		case AXIS_Y: forward_axis = bone_t.getAxisY(); break;
		case AXIS_Z: forward_axis = bone_t.getAxisZ(); break;
		case AXIS_NX: forward_axis = -bone_t.getAxisX(); break;
		case AXIS_NY: forward_axis = -bone_t.getAxisY(); break;
		case AXIS_NZ: forward_axis = -bone_t.getAxisZ(); break;
	}
	vec3 position = projectOntoPlane(bone_t.getTranslate(), vec3_up);
	forward_axis = projectOntoPlane(forward_axis, vec3_up);
	forward_axis.normalize();
	vec3 right_axis = cross(forward_axis, vec3_up);
	right_axis.normalize();

	proj_bone_t.setColumn3(0, right_axis);
	proj_bone_t.setColumn3(1, forward_axis);
	proj_bone_t.setColumn3(2, vec3_up);
	proj_bone_t.setColumn3(3, position);
}

void AnimationBonesRootMotion::init()
{
	skinned = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_node.get());
	if (skinned.isValid() == false)
	{
		Log::error("AnimationBonesRootMotion::init(): skinned is null\n");
		return;
	}

	skinned_root_motion = checked_ptr_cast<ObjectMeshSkinned>(mesh_skinned_root_motion_node.get());
	if (skinned_root_motion.isValid() == false)
	{
		Log::error("AnimationBonesRootMotion::init(): root motion skinned is null\n");
		return;
	}

	skinned->stop();
	skinned_root_motion->stop();

	skinned->setAnimPath(animation_path.get());
	skinned_root_motion->setAnimPath(animation_path.get());

	frame_updater.init(skinned);
	root_motion_frame_updater.init(skinned_root_motion, root_bone_name);

	Visualizer::setEnabled(true);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED);
}

void AnimationBonesRootMotion::update()
{
	auto render_transform = [](const Mat4 t)
	{
		Vec3 p = t.getTranslate();
		Visualizer::renderVector(p, p + t.getAxisX(), vec4_red);
		Visualizer::renderVector(p, p + t.getAxisY(), vec4_green);
		Visualizer::renderVector(p, p + t.getAxisZ(), vec4_blue);
	};

	render_transform(skinned->getWorldTransform());
	render_transform(skinned_root_motion->getWorldTransform());
}

void AnimationBonesRootMotion::post_update()
{
	root_motion_frame_updater.applyRootMotion(skinned_root_motion);
}

void AnimationBonesRootMotion::shutdown()
{
	Visualizer::setEnabled(false);
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);
}
