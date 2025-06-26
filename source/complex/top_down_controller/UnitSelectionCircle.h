#pragma once

#include <UnigineComponentSystem.h>

class UnitSelectionCircle : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UnitSelectionCircle, Unigine::ComponentBase);

	COMPONENT_INIT(init);

	PROP_PARAM(File, selectionCircle, nullptr, "NodeRefence of selection circle");
	PROP_PARAM(Vec3, offset, Unigine::Math::vec3(0, 0, 0.01), nullptr, "Offset of selection circle from the center of the unit");

	void setSelected(bool value);
	bool isSelected() { return selected; }

protected:
	void init();
	Unigine::NodePtr selection_circle;

private:
	bool selected = false;
};