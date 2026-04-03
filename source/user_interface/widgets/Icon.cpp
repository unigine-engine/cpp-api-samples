// Demonstrates WidgetIcon as a toggleable image button. When clicked, the icon
// toggles between on/off states and displays the current state on the console
// overlay via the EventClicked callback.

#include "Icon.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Icon);

using namespace Unigine;
using namespace Math;

// Icon widget is created with toggle mode enabled and click callback is connected.
void Icon::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_icon = WidgetIcon::create(gui, icon_image.get());

	gui->addChild(widget_icon, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_icon->setPosition(position.get().x, position.get().y);
	widget_icon->setWidth(size.get().x);
	widget_icon->setHeight(size.get().y);
	// Enable toggle behavior for on/off state switching
	widget_icon->setToggleable(true);

	// Lambda callback displays toggle state on click
	widget_icon->getEventClicked().connect(*this, [this]() {
		String msg = String("Icon: ") + (widget_icon->isToggled() ? "True" : "False");
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void Icon::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_icon);

		widget_icon.deleteLater();
	}

	Console::setOnscreen(false);
}
