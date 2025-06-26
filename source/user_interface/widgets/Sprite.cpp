#include "Sprite.h"

REGISTER_COMPONENT(Sprite);

using namespace Unigine;
using namespace Math;

void Sprite::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_sprite = WidgetSprite::create(gui, sprite_image.get());

	gui->addChild(widget_sprite, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_sprite->setPosition(position.get().x, position.get().y);
	widget_sprite->setWidth(size.get().x);
	widget_sprite->setHeight(size.get().y);
}

void Sprite::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_sprite);

		widget_sprite.deleteLater();
	}
}
