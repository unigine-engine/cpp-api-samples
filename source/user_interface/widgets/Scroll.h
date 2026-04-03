// Demonstrates WidgetScroll as a standalone scrollbar control. The scrollbar
// orientation is set to horizontal, and value changes trigger EventChanged
// callback that shows the current scroll position on the console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a horizontal scrollbar widget with value change tracking.
class Scroll : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Scroll, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(500, 150));

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Scrollbar widget instance
	Unigine::WidgetScrollPtr widget_scroll;
};
