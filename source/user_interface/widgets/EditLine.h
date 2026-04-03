// Demonstrates single-line text input using WidgetEditLine. Text changes are
// monitored via EventChanged callback and displayed on the console overlay.
// Widget size and font are configured through component properties.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a single-line text input field with change notification.
class EditLine : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(EditLine, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(750, 50));
	// Widget dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(150, 30));
	// Initial placeholder text
	PROP_PARAM(String, text, "Enter text...");
	// Font size for input text
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Edit line widget instance
	Unigine::WidgetEditLinePtr widget_editline;
};
