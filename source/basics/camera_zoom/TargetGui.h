#pragma once
#include "UnigineComponentSystem.h"

class TargetGui : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TargetGui, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	void update();
private:

	Unigine::ObjectGuiPtr object_gui;
	Unigine::GuiPtr gui;
	Unigine::WidgetLabelPtr distance_label;

	void init();
};