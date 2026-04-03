// Demonstrates WidgetIcon as a toggleable image button. When clicked, the icon
// toggles between on/off states and displays the current state on the console
// overlay via the EventClicked callback.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a clickable icon widget with toggle functionality.
class Icon : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Icon, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(500, 450));
	// Icon dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(32, 32));
	// Image file for icon texture
	PROP_PARAM(File, icon_image);

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Icon widget instance
	Unigine::WidgetIconPtr widget_icon;
};
