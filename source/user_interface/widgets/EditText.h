// Demonstrates multi-line text input using WidgetEditText. Text changes are
// monitored via EventChanged callback and displayed on the console overlay.
// Widget dimensions and font size are configured through component properties.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a multi-line text input field with change notification.
class EditText : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EditText, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(250, 150));
	// Widget dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(150, 100));
	// Initial placeholder text
	PROP_PARAM(String, text, "Enter text...");
	// Font size for input text
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Multi-line edit text widget instance
	Unigine::WidgetEditTextPtr widget_edittext;
};
