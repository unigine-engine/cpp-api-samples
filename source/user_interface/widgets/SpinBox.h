// Demonstrates WidgetSpinBox attached to a WidgetEditLine for numeric input with
// increment/decrement buttons. The spinbox is layered above the edit line using
// widget ordering, and value changes are shown on the console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a spinbox control attached to an edit line for numeric input.
class SpinBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SpinBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(625, 300));

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Spinbox widget with increment/decrement buttons
	Unigine::WidgetSpinBoxPtr widget_spinbox;
	// Edit line widget for displaying numeric value
	Unigine::WidgetEditLinePtr widget_editline;
};
