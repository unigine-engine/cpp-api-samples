#pragma once
#include <UnigineComponentSystem.h>
#include "Interactable.h"

class ButtonReactionSample : public Unigine::ComponentBase
{
	public:
	COMPONENT_DEFINE(ButtonReactionSample, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Vec3, position);
	PROP_PARAM(Vec3, rotation);
	PROP_PARAM(Node, interatable_node);

private:

	void init();
	void update();

	Unigine::Math::Vec3 initial_position;
	Unigine::Math::quat initial_rotation;
	Unigine::Math::quat final_rotation;
	Interactable* interactable_comp = nullptr;
};

