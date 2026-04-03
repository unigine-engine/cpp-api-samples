// Demonstrates WidgetListBox for displaying a scrollable list of selectable items.
// Items are added programmatically, and selection changes trigger EventChanged
// callback that shows the selected item text on the console overlay.

#include "ListBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(ListBox);

using namespace Unigine;
using namespace Math;

// List box widget is created with sample items and selection callback is connected.
void ListBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_listbox = WidgetListBox::create(gui);

	gui->addChild(widget_listbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_listbox->setPosition(position.get().x, position.get().y);
	widget_listbox->setFontSize(font_size.get());
	widget_listbox->setFontOutline(1);
	// Populate list with sample items
	widget_listbox->addItem("item 0");
	widget_listbox->addItem("item 1");
	widget_listbox->addItem("item 2");

	// Lambda callback displays selected item text
	widget_listbox->getEventChanged().connect(*this, [this]() {
		String msg = String("ListBox: ") + widget_listbox->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void ListBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_listbox);

		widget_listbox.deleteLater();
	}

	Console::setOnscreen(false);
}
