// Demonstrates WidgetSprite for displaying 2D images in the UI. The sprite is
// loaded from an image file specified via component property, with configurable
// position and dimensions.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a sprite widget for displaying 2D images.
class Sprite : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Sprite, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(275, 450));
	// Widget dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	// Path to the image file for the sprite
	PROP_PARAM(File, sprite_image);

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Sprite widget instance
	Unigine::WidgetSpritePtr widget_sprite;
};