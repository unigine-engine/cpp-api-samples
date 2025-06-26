#pragma once
#include <UnigineComponentSystem.h>

class Label : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Label, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(800, 150));
	PROP_PARAM(String, text, "Label");
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetLabelPtr widget_label;
};