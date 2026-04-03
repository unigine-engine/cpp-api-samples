// Demonstrates WidgetCheckBox creation and event handling. State changes trigger
// an EventChanged callback that displays the current checked state on the
// console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a toggleable checkbox widget with label text.
class CheckBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CheckBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(450, 50));
	// Label text displayed next to checkbox
	PROP_PARAM(String, text, "Check Me");
	// Font size for label text
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Checkbox widget instance
	Unigine::WidgetCheckBoxPtr widget_checkbox;
};
