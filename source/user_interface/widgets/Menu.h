// Demonstrates WidgetMenuBar with nested WidgetMenuBox dropdowns. Each menu
// bar item opens a popup menu with selectable items, and selection events are
// handled via EventClicked callbacks that display the chosen item on console.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a menu bar with dropdown popup menus.
class Menu : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Menu, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(450, 300));
	// Font size for menu items
	PROP_PARAM(Int, font_size, 16)
	// Highlight color for hovered menu items
	PROP_PARAM(Color, selection_color, Unigine::Math::vec4(0.3f, 0.3f, 0.3f, 1.0f));

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Menu bar widget instance
	Unigine::WidgetMenuBarPtr widget_menubar;
};
