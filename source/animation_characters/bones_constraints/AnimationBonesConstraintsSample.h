#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates Inverse Kinematics (IK) with bone rotation constraints.
// A target can be dragged in 3D space, and the bone chain automatically adapts
// while respecting the specified angle limits for yaw/pitch/roll axes.
class AnimationBonesConstraintsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesConstraintsSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to ObjectMeshSkinned node containing the skeletal mesh
	PROP_PARAM(Node, mesh_skinned_node);
	// List of bone names forming the IK chain (order matters: from root to tip)
	PROP_ARRAY(String, ik_chain_bones);

	// Structure defining rotation constraints for an individual bone
	struct Constraint: Unigine::ComponentStruct
	{
		// Name of the bone to apply constraints to
		PROP_PARAM(String, name);

		// Local rotation axes of the bone (defined in the bone's local coordinate system)
		PROP_PARAM(Vec3, yaw_axis, Unigine::Math::vec3_forward);
		PROP_PARAM(Vec3, pitch_axis, -Unigine::Math::vec3_right);
		PROP_PARAM(Vec3, roll_axis, Unigine::Math::vec3_up);

		// Allowed angle range for each axis (in degrees)
		PROP_PARAM(Float, yaw_min_angle, -180.0f);
		PROP_PARAM(Float, yaw_max_angle, 180.0f);

		PROP_PARAM(Float, pitch_min_angle, -90.0f);
		PROP_PARAM(Float, pitch_max_angle, 90.0f);

		// Roll is locked by default (min == max == 0)
		PROP_PARAM(Float, roll_min_angle, -0.0f);
		PROP_PARAM(Float, roll_max_angle, 0.0f);
	};

	PROP_ARRAY_STRUCT(Constraint, constraints);

private:
	void init();
	void update();
	void shutdown();

private:
	// Widget manipulator for interactive dragging of the IK target in 3D space
	Unigine::WidgetManipulatorTranslatorPtr target_translator;

	Unigine::ObjectMeshSkinnedLegacyPtr skinned;
	// ID of the created IK chain (-1 means the chain is not initialized)
	int chain_id{-1};
};
