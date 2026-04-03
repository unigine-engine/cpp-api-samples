#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// This component demonstrates LookAt chains for aiming bones at a target.
// Multiple bones (e.g., spine and head) can participate in the chain with different weights,
// distributing the rotation across the chain for a natural look.
class AnimationBonesLookAtSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesLookAtSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to ObjectMeshSkinned node containing the skeletal mesh
	PROP_PARAM(Node, mesh_skinned_node);

	// Configuration for each bone participating in the LookAt chain
	struct Bone: Unigine::ComponentStruct
	{
		// Name of the bone to include in the LookAt chain
		PROP_PARAM(String, name);
		// Forward axis of the bone (direction that should point toward the target)
		PROP_PARAM(Vec3, axis, Unigine::Math::vec3_forward);
		// Up axis of the bone (used for twist/roll orientation)
		PROP_PARAM(Vec3, up, Unigine::Math::vec3_up);
		// Contribution weight: higher values = bone rotates more toward target
		PROP_PARAM(Float, weight, 1.0f);
	};

	PROP_ARRAY_STRUCT(Bone, bones);

private:
	void init();
	void update();
	void shutdown();

private:
	// Manipulator for the look-at target position
	Unigine::WidgetManipulatorTranslatorPtr target_translator;
	// Manipulator for the pole vector (controls the "up" orientation of the chain)
	Unigine::WidgetManipulatorTranslatorPtr pole_translator;

	Unigine::ObjectMeshSkinnedLegacyPtr skinned;
	// ID of the created LookAt chain (-1 means the chain is not initialized)
	int chain_id{-1};
};
