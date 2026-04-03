// Demonstrates WidgetLabel for displaying static text. The label is positioned
// and styled via component properties with no event handling required for this
// non-interactive widget type.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a static text label widget for display purposes.
class Label : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Label, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(800, 150));
	// Text content to display
	PROP_PARAM(String, text, "Label");
	// Font size for label text
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Label widget instance
	Unigine::WidgetLabelPtr widget_label;
};