// Demonstrates WidgetListBox for displaying a scrollable list of selectable
// items. Items are added programmatically, and selection changes trigger
// EventChanged callback that shows the selected item text on the console.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a scrollable list widget for item selection.
class ListBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ListBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(300, 300));
	// Font size for list items
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// List box widget instance
	Unigine::WidgetListBoxPtr widget_listbox;
};
