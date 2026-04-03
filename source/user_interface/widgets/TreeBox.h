// Demonstrates WidgetTreeBox for hierarchical item display. Parent-child
// relationships are established via addItemChild, creating expandable tree
// nodes. Selection changes trigger EventChanged callback for console output.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a tree view widget with expandable hierarchical nodes.
class TreeBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TreeBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(775, 300));
	// Font size for tree item labels
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Tree box widget instance
	Unigine::WidgetTreeBoxPtr widget_treebox;
};
