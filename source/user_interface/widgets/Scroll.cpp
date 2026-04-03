// Demonstrates WidgetScroll as a standalone scrollbar control. The scrollbar
// orientation is set to horizontal, and value changes trigger EventChanged
// callback that shows the current scroll position on the console overlay.

#include "Scroll.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Scroll);

using namespace Unigine;
using namespace Math;

// Horizontal scrollbar widget is created with value change callback.
void Scroll::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_scroll = WidgetScroll::create(gui);

	gui->addChild(widget_scroll, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_scroll->setPosition(position.get().x, position.get().y);
	// Set horizontal orientation (0 = horizontal, 1 = vertical)
	widget_scroll->setOrientation(0);

	// Lambda callback displays current scroll value on change
	widget_scroll->getEventChanged().connect(*this, [this]() {
		String msg = String("Scroll: ") + String::itoa(widget_scroll->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void Scroll::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_scroll);

		widget_scroll.deleteLater();
	}

	Console::setOnscreen(false);
}
