// Demonstrates WidgetSprite for displaying 2D images in the UI. The sprite is
// loaded from an image file specified via component property, with configurable
// position and dimensions.

#include "Sprite.h"

REGISTER_COMPONENT(Sprite);

using namespace Unigine;
using namespace Math;

// Sprite widget is created from the specified image file with configured dimensions.
void Sprite::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	// Create sprite from image file
	widget_sprite = WidgetSprite::create(gui, sprite_image.get());

	gui->addChild(widget_sprite, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_sprite->setPosition(position.get().x, position.get().y);
	widget_sprite->setWidth(size.get().x);
	widget_sprite->setHeight(size.get().y);
}

// Widget is removed from GUI and resources are released.
void Sprite::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_sprite);

		widget_sprite.deleteLater();
	}
}
