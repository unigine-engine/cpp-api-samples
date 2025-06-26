#pragma once
#include <UnigineComponentSystem.h>

class ListBox : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ListBox, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(300, 300));
	PROP_PARAM(Int, font_size, 16)

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetListBoxPtr widget_listbox;
};
