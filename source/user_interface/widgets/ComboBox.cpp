// Demonstrates WidgetComboBox as a dropdown selection widget. Items are added
// programmatically, and selection changes trigger EventChanged callback that
// shows the selected item text on the console overlay.

#include "ComboBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(ComboBox);

using namespace Unigine;
using namespace Math;

// Combobox widget is created with sample items and selection callback is connected.
void ComboBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_combobox = WidgetComboBox::create(gui);

	gui->addChild(widget_combobox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_combobox->setPosition(position.get().x, position.get().y);
	widget_combobox->setFontSize(font_size.get());

	// Populate dropdown with sample items
	widget_combobox->addItem("item 0");
	widget_combobox->addItem("item 1");
	widget_combobox->addItem("item 2");

	// Lambda callback displays selected item text
	widget_combobox->getEventChanged().connect(*this, [this]() {
		String msg = String("ComboBox: ") + widget_combobox->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void ComboBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_combobox);

		widget_combobox.deleteLater();
	}

	Console::setOnscreen(false);
}
