#pragma once
#include <UnigineComponentSystem.h>

class Menu : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Menu, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(450, 300));
	PROP_PARAM(Int, font_size, 16)
	PROP_PARAM(Color, selection_color, Unigine::Math::vec4(0.3f, 0.3f, 0.3f, 1.0f));

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetMenuBarPtr widget_menubar;
};
