// Visual selection indicator for RTS units. Loads and parents a selection circle
// node to the unit, toggling its visibility based on selection state.

#pragma once

#include <UnigineComponentSystem.h>

// Marker component for selectable units. Loads a visual indicator (selection circle)
// that appears beneath the unit when selected.
class UnitSelectionCircle : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UnitSelectionCircle, Unigine::ComponentBase);

	COMPONENT_INIT(init);

	PROP_PARAM(File, selectionCircle, nullptr, "NodeRefence of selection circle");
	PROP_PARAM(Vec3, offset, Unigine::Math::vec3(0, 0, 0.01), nullptr, "Offset of selection circle from the center of the unit");

	void setSelected(bool value);			// Sets selection state and toggles circle visibility
	bool isSelected() { return selected; }	// Returns current selection state

protected:
	void init();
	Unigine::NodePtr selection_circle;	// The loaded visual indicator node

private:
	bool selected = false;	// Current selection state
};