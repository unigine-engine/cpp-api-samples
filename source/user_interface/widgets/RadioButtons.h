#pragma once
#include <UnigineComponentSystem.h>

class RadioButtons : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RadioButtons, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, widget_position, Unigine::Math::vec2(600, 450));
	PROP_PARAM(Int, font_size, 16);
	PROP_PARAM(Int, horizontal_layout_space, 4);
	PROP_PARAM(Int, vertical_layout_space, 4);
	PROP_PARAM(String, first_rbutton_text, "Check Me");
	PROP_PARAM(String, second_rbutton_text, "Or Me");

private:
	void init();
	void shutdown();

	Unigine::WidgetVBoxPtr p_vertical_layout;

	Unigine::WidgetCheckBoxPtr p_first_rbutton = nullptr;
	Unigine::WidgetCheckBoxPtr p_second_rbutton = nullptr;
};
