#include "ListBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(ListBox);

using namespace Unigine;
using namespace Math;

void ListBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_listbox = WidgetListBox::create(gui);

	gui->addChild(widget_listbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_listbox->setPosition(position.get().x, position.get().y);
	widget_listbox->setFontSize(font_size.get());
	widget_listbox->setFontOutline(1);
	widget_listbox->addItem("item 0");
	widget_listbox->addItem("item 1");
	widget_listbox->addItem("item 2");

	widget_listbox->getEventChanged().connect(*this, [this]() {
		String msg = String("ListBox: ") + widget_listbox->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void ListBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_listbox);

		widget_listbox.deleteLater();
	}

	Console::setOnscreen(false);
}
