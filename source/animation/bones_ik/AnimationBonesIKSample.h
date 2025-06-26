#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesIKSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesIKSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_ARRAY(String, bones);

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
