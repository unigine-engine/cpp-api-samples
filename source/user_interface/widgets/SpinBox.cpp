// Demonstrates WidgetSpinBox attached to a WidgetEditLine for numeric input with
// increment/decrement buttons. The spinbox is layered above the edit line using
// widget ordering, and value changes are shown on the console overlay.

#include "SpinBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(SpinBox);

using namespace Unigine;
using namespace Math;

// Spinbox is created and attached to an edit line with value change callback.
void SpinBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	// Create edit line for displaying numeric value
	widget_editline = WidgetEditLine::create(gui, "0");
	widget_editline->setPosition(position.get().x, position.get().y);
	widget_editline->setFontOutline(1);

	gui->addChild(widget_editline, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	// Create spinbox and layer it above the edit line
	widget_spinbox = WidgetSpinBox::create(gui);
	widget_spinbox->setOrder(widget_editline->getOrder() + 1);
	// Attach spinbox to edit line for coordinated positioning
	widget_editline->addAttach(widget_spinbox);

	gui->addChild(widget_spinbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	// Lambda callback displays current spinbox value on change
	widget_spinbox->getEventChanged().connect(*this, [this]() {
		String msg = String("SpinBox: ") + String::itoa(widget_spinbox->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widgets are removed from GUI and resources are released.
void SpinBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_editline);
		gui->removeChild(widget_spinbox);

		widget_editline.deleteLater();
		widget_spinbox.deleteLater();
	}

	Console::setOnscreen(false);
}
