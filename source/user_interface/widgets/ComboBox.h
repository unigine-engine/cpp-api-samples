// Demonstrates WidgetComboBox as a dropdown selection widget. Items are added
// programmatically, and selection changes trigger EventChanged callback that
// shows the selected item text on the console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a dropdown list widget for item selection.
class ComboBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ComboBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(600, 50));
	// Font size for dropdown items
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Combobox widget instance
	Unigine::WidgetComboBoxPtr widget_combobox;
};
