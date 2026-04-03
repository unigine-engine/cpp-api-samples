// Displays an in-world GUI panel attached to a target object showing
// distance from camera and label size. Updates distance each frame.

#pragma once
#include "UnigineComponentSystem.h"

// In-world GUI showing target info (name, distance, size).
class TargetGui : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TargetGui, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	void update();

private:
	Unigine::ObjectGuiPtr object_gui;		// ObjectGui this component is attached to
	Unigine::GuiPtr gui;					// GUI context for widget creation
	Unigine::WidgetLabelPtr distance_label;	// Updated each frame with current distance

	void init();
};