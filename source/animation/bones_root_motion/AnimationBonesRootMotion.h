#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

struct FrameUpdater
{
	void init(Unigine::ObjectMeshSkinnedPtr &skinned); 
private:
		Unigine::EventConnection skinned_connection;
};

struct RootMotionFrameUpdater
{
	void init(Unigine::ObjectMeshSkinnedPtr &skinned, const char *root_bone_name, Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z);
	void applyRootMotion(Unigine::ObjectMeshSkinnedPtr &skinned);

	void update_proj_bone_transform(const Unigine::Math::mat4 &bone_t, Unigine::Math::AXIS axis_forward);

	int root_bone{-1};
	Unigine::Math::mat4 proj_bone_t{Unigine::Math::mat4_identity};
	Unigine::Math::mat4 prev_proj_bone_t{Unigine::Math::mat4_identity};

	Unigine::Math::mat4 root_motion_delta{Unigine::Math::mat4_identity};
	Unigine::Math::mat4 last_frame_delta{Unigine::Math::mat4_identity};
	long long root_motion_delta_frame{0};
	Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z;
private:
	Unigine::EventConnection skinned_connection;
};

class AnimationBonesRootMotion : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesRootMotion, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_PARAM(Node, mesh_skinned_root_motion_node);
	PROP_PARAM(File, animation_path);
	PROP_PARAM(String, root_bone_name);
	PROP_PARAM(Switch, axis_forward, 2, "X,Y,Z,NX,NY,NZ")

private:
	void init();
	void update();
	void post_update();
	void shutdown();

private:
	FrameUpdater frame_updater;
	RootMotionFrameUpdater root_motion_frame_updater;

	Unigine::ObjectMeshSkinnedPtr skinned;
	Unigine::ObjectMeshSkinnedPtr skinned_root_motion;
};
