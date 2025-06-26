#pragma once
#include <UnigineComponentSystem.h>

class Scroll : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Scroll, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(500, 150));

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetScrollPtr widget_scroll;
};
