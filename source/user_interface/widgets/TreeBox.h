#pragma once
#include <UnigineComponentSystem.h>

class TreeBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TreeBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(775, 300));
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetTreeBoxPtr widget_treebox;
};
