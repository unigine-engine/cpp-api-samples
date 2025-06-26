#include "Crosshair.h"

REGISTER_COMPONENT(Crosshair);

using namespace Unigine;

void Crosshair::init()
{
	auto gui = Gui::getCurrent();

	crosshair = WidgetSprite::create(gui, crosshair_image);
	crosshair->setWidth(crosshair_size.get().x);
	crosshair->setHeight(crosshair_size.get().y);
	gui->addChild(crosshair, Gui::ALIGN_CENTER | Gui::ALIGN_OVERLAP);
}

void Crosshair::shutdown()
{
	crosshair.deleteLater();
}
