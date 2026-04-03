// Demonstrates WidgetButton creation with configurable size, position, and font.
// Click events are handled via EventClicked callback that displays a message
// on the console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a clickable button widget with customizable appearance.
class Button : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Button, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(250, 50));
	// Button dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	// Label text displayed on button
	PROP_PARAM(String, text, "Press Me");
	// Font size for button text
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Button widget instance
	Unigine::WidgetButtonPtr widget_button;
};
