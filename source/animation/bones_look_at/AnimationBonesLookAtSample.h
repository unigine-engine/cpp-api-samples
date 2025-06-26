#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesLookAtSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesLookAtSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);

	struct Bone: Unigine::ComponentStruct
	{
		PROP_PARAM(String, name);
		PROP_PARAM(Vec3, axis, Unigine::Math::vec3_forward);
		PROP_PARAM(Vec3, up, Unigine::Math::vec3_up);
		PROP_PARAM(Float, weight, 1.0f);
	};

	PROP_ARRAY_STRUCT(Bone, bones);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::WidgetManipulatorTranslatorPtr target_translator;
	Unigine::WidgetManipulatorTranslatorPtr pole_translator;

	Unigine::ObjectMeshSkinnedPtr skinned;
	int chain_id{-1};
};
