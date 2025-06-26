#include "Scroll.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Scroll);

using namespace Unigine;
using namespace Math;

void Scroll::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_scroll = WidgetScroll::create(gui);

	gui->addChild(widget_scroll, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_scroll->setPosition(position.get().x, position.get().y);
	widget_scroll->setOrientation(0);

	widget_scroll->getEventChanged().connect(*this, [this]() {
		String msg = String("Scroll: ") + String::itoa(widget_scroll->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void Scroll::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_scroll);

		widget_scroll.deleteLater();
	}

	Console::setOnscreen(false);
}
