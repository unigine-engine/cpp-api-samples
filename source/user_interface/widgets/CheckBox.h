#pragma once
#include <UnigineComponentSystem.h>

class CheckBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CheckBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(450, 50));
	PROP_PARAM(String, text, "Check Me");
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetCheckBoxPtr widget_checkbox;
};
