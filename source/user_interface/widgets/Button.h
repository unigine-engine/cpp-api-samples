#pragma once
#include <UnigineComponentSystem.h>

class Button : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Button, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(250, 50));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	PROP_PARAM(String, text, "Press Me");
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetButtonPtr widget_button;
};
