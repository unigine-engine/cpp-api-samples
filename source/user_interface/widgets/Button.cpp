#include "Button.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Button);

using namespace Unigine;
using namespace Math;

void Button::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_button = WidgetButton::create(gui, text.get());

	gui->addChild(widget_button, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_button->setPosition(position.get().x, position.get().y);
	widget_button->setWidth(size.get().x);
	widget_button->setHeight(size.get().y);
	widget_button->setFontSize(font_size.get());

	widget_button->getEventClicked().connect(*this, []() {
		Console::onscreenMessageLine("Button Clicked!");
		});

	Console::setOnscreen(true);
}

void Button::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_button);

		widget_button.deleteLater();
	}

	Console::setOnscreen(false);
}
