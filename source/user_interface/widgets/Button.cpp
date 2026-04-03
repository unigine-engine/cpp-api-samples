// Demonstrates WidgetButton creation with configurable size, position, and font.
// The EventClicked callback displays a message on the console overlay when the
// button is pressed.

#include "Button.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Button);

using namespace Unigine;
using namespace Math;

// Button widget is created with text label and click callback is connected.
void Button::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_button = WidgetButton::create(gui, text.get());

	// Add to GUI with overlap alignment for absolute positioning
	gui->addChild(widget_button, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_button->setPosition(position.get().x, position.get().y);
	widget_button->setWidth(size.get().x);
	widget_button->setHeight(size.get().y);
	widget_button->setFontSize(font_size.get());

	// Lambda callback displays message on console when clicked
	widget_button->getEventClicked().connect(*this, []() {
		Console::onscreenMessageLine("Button Clicked!");
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void Button::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_button);

		widget_button.deleteLater();
	}

	Console::setOnscreen(false);
}
