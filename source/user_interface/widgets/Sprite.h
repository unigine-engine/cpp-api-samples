#pragma once
#include <UnigineComponentSystem.h>

class Sprite : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Sprite, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Vec2, position, Unigine::Math::vec2(275, 450));
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	PROP_PARAM(File, sprite_image);

private:
	void init();
	void shutdown();

	Unigine::GuiPtr gui;
	Unigine::WidgetSpritePtr widget_sprite;
};