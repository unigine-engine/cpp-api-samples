#pragma once
#include <UnigineComponentSystem.h>

class Icon : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Icon, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(500, 450));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(32, 32));
	PROP_PARAM(File, icon_image);

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetIconPtr widget_icon;
};
