#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates foot placement on uneven surfaces using IK chains.
// Raycasting is used to detect ground contact: each frame, a ray is cast downward
// from each foot to find the surface. The foot's position and rotation are then
// adjusted to match the surface height and normal.
class AnimationBonesSimpleFootPlacementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesSimpleFootPlacementSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to ObjectMeshSkinned node containing the skeletal mesh
	PROP_PARAM(Node, mesh_skinned_node);
	// Bone chain for left leg (e.g., thigh -> shin -> foot)
	PROP_ARRAY(String, left_leg_bones);
	// Bone chain for right leg
	PROP_ARRAY(String, right_leg_bones);
	// Height offset from ground to foot pivot point
	PROP_PARAM(Float, foot_height, 0.1f);
	// Optional movable platform/obstacle for testing foot placement
	PROP_PARAM(Node, obstacle);

private:
	enum FOOT
	{
		FOOT_LEFT = 0,
		FOOT_RIGHT,
		FOOT_NUM,
	};

private:
	void init();
	void update();
	void shutdown();

	// Called before IK solving to update foot targets based on raycast results
	void on_begin_ik_solvers();

private:
	// Manipulators for moving/rotating the obstacle platform
	Unigine::WidgetManipulatorTranslatorPtr translator;
	Unigine::WidgetManipulatorRotatorPtr rotator;

	Unigine::ObjectMeshSkinnedLegacyPtr skinned;
	int chain_ids[FOOT_NUM];
	// Last bone in each chain (the foot bone used as end effector)
	int effector_bones[FOOT_NUM];
};
