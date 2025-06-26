#pragma once
#include <UnigineComponentSystem.h>

class Slider : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Slider, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(600, 150));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	PROP_PARAM(Int, button_width, 30)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetSliderPtr widget_slider;
};
