#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class AnimationBonesSimpleFootPlacementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AnimationBonesSimpleFootPlacementSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, mesh_skinned_node);
	PROP_ARRAY(String, left_leg_bones);
	PROP_ARRAY(String, right_leg_bones);
	PROP_PARAM(Float, foot_height, 0.1f);
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

	void on_begin_ik_solvers();

private:
	Unigine::WidgetManipulatorTranslatorPtr translator;
	Unigine::WidgetManipulatorRotatorPtr rotator;

	Unigine::ObjectMeshSkinnedPtr skinned;
	int chain_ids[FOOT_NUM];
	int effector_bones[FOOT_NUM];
};
