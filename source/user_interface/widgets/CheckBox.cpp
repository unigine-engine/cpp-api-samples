// Demonstrates WidgetCheckBox creation and event handling. The checkbox state
// change triggers an EventChanged callback that displays the current checked
// state on the console overlay.

#include "CheckBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(CheckBox);

using namespace Unigine;
using namespace Math;

// Checkbox widget is created and state change callback is connected.
void CheckBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_checkbox = WidgetCheckBox::create(gui, text.get());

	gui->addChild(widget_checkbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_checkbox->setPosition(position.get().x, position.get().y);
	widget_checkbox->setFontSize(font_size.get());

	// Lambda callback displays checked state on toggle
	widget_checkbox->getEventChanged().connect(*this, [this]() {
		String msg = String("CheckBox: ") + (widget_checkbox->isChecked() ? "True" : "False");
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void CheckBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_checkbox);

		widget_checkbox.deleteLater();
	}

	Console::setOnscreen(false);
}
