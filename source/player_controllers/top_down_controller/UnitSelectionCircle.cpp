// Visual indicator for selected units. Loads a selection circle node
// and parents it to the unit, toggling visibility based on selection state.

#include "UnitSelectionCircle.h"
#include <UniginePrimitives.h>
REGISTER_COMPONENT(UnitSelectionCircle);

using namespace Unigine;
using namespace Math;

// Sets selection state and shows/hides the selection circle accordingly.
void UnitSelectionCircle::setSelected(bool value)
{
	if (!selection_circle)
		return;

	selected = value;
	selection_circle->setEnabled(selected);
}

// Loads the selection circle node, parents it to this unit, and hides it initially.
void UnitSelectionCircle::init()
{
	selection_circle = World::loadNode(selectionCircle.get());
	if (!selection_circle)
	{
		Log::error("UnitSelectionCircle::init(): cannot load node '%s'\n", selectionCircle.get());
		return;
	}

	selection_circle->setParent(node);
	selection_circle->setPosition(Vec3(offset.get()));
	selection_circle->setEnabled(false);
}
