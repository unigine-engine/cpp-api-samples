#include "UnitSelectionCircle.h"
#include <UniginePrimitives.h>
REGISTER_COMPONENT(UnitSelectionCircle);

using namespace Unigine;
using namespace Math;

void UnitSelectionCircle::setSelected(bool value)
{
	if (!selection_circle)
		return;

	selected = value;
	selection_circle->setEnabled(selected);
}

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
