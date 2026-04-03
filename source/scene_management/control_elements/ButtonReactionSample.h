// Reactor component that transforms a node based on interactable state.
// Interpolates between initial and target transforms using the interactable value.
// Demonstrates decoupled control-reactor pattern for simulation systems.

#pragma once
#include <UnigineComponentSystem.h>
#include "Interactable.h"

// Transforms node based on linked interactable's normalized value.
class ButtonReactionSample : public Unigine::ComponentBase
{
	public:
	COMPONENT_DEFINE(ButtonReactionSample, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Position offset when interactable value is 1.0
	PROP_PARAM(Vec3, position);
	// Euler rotation offset when interactable value is 1.0
	PROP_PARAM(Vec3, rotation);
	// Reference to the interactable that drives this reactor
	PROP_PARAM(Node, interatable_node);

private:

	void init();
	void update();

	// Starting position for interpolation
	Unigine::Math::Vec3 initial_position;
	// Starting rotation for interpolation
	Unigine::Math::quat initial_rotation;
	// Target rotation when value is 1.0
	Unigine::Math::quat final_rotation;
	// Cached pointer to interactable component
	Interactable* interactable_comp = nullptr;
};

