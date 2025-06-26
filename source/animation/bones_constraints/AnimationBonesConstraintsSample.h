#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesConstraintsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesConstraintsSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_ARRAY(String, ik_chain_bones);

	struct Constraint: Unigine::ComponentStruct
	{
		PROP_PARAM(String, name);
		PROP_PARAM(Vec3, yaw_axis, Unigine::Math::vec3_forward);
		PROP_PARAM(Vec3, pitch_axis, -Unigine::Math::vec3_right);
		PROP_PARAM(Vec3, roll_axis, Unigine::Math::vec3_up);

		PROP_PARAM(Float, yaw_min_angle, -180.0f);
		PROP_PARAM(Float, yaw_max_angle, 180.0f);

		PROP_PARAM(Float, pitch_min_angle, -90.0f);
		PROP_PARAM(Float, pitch_max_angle, 90.0f);

		PROP_PARAM(Float, roll_min_angle, -0.0f);
		PROP_PARAM(Float, roll_max_angle, 0.0f);
	};

	PROP_ARRAY_STRUCT(Constraint, constraints);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::WidgetManipulatorTranslatorPtr target_translator;

	Unigine::ObjectMeshSkinnedPtr skinned;
	int chain_id{-1};
};
