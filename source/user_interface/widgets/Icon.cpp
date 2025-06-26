#include "Icon.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Icon);

using namespace Unigine;
using namespace Math;

void Icon::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_icon = WidgetIcon::create(gui, icon_image.get());

	gui->addChild(widget_icon, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_icon->setPosition(position.get().x, position.get().y);
	widget_icon->setWidth(size.get().x);
	widget_icon->setHeight(size.get().y);
	widget_icon->setToggleable(true);

	widget_icon->getEventClicked().connect(*this, [this]() {
		String msg = String("Icon: ") + (widget_icon->isToggled() ? "True" : "False");
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void Icon::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_icon);

		widget_icon.deleteLater();
	}

	Console::setOnscreen(false);
}
