// Demonstrates WidgetMenuBar with nested WidgetMenuBox dropdowns. Each menu bar
// item opens a popup menu with selectable items, and selection events are handled
// via EventClicked callbacks that display the chosen item on console.

#include "Menu.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Menu);

using namespace Unigine;
using namespace Math;

// Menu bar is created with three dropdown menus, each with click callbacks.
void Menu::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_menubar = WidgetMenuBar::create(gui);

	gui->addChild(widget_menubar, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_menubar->setPosition(position.get().x, position.get().y);
	widget_menubar->setFontSize(font_size.get());
	widget_menubar->setFontOutline(1);
	widget_menubar->setSelectionColor(selection_color.get());
	// Add menu bar items (labels for dropdown menus)
	widget_menubar->addItem("File 0");
	widget_menubar->addItem("File 1");
	widget_menubar->addItem("File 2");

	// Create File dropdown menu
	WidgetMenuBoxPtr file_menu_box = WidgetMenuBox::create(gui);
	file_menu_box->setFontSize(font_size.get());
	file_menu_box->setFontOutline(1);
	file_menu_box->addItem("File 0");
	file_menu_box->addItem("File 1");
	file_menu_box->addItem("File 2");
	widget_menubar->setItemMenu(0, file_menu_box);

	file_menu_box->getEventClicked().connect(*this, [file_menu_box](){
		String msg = String("Menu: ") + file_menu_box->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	// Add edit menubox
	WidgetMenuBoxPtr edit_menu_box = WidgetMenuBox::create(gui);
	edit_menu_box->setFontSize(font_size.get());
	edit_menu_box->setFontOutline(1);
	edit_menu_box->addItem("Edit 0");
	edit_menu_box->addItem("Edit 1");
	edit_menu_box->addItem("Edit 2");
	widget_menubar->setItemMenu(1, edit_menu_box);

	edit_menu_box->getEventClicked().connect(*this, [edit_menu_box](){
		String msg = String("Menu: ") + edit_menu_box->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	// Add help menubox
	WidgetMenuBoxPtr help_menu_box = WidgetMenuBox::create(gui);
	help_menu_box->setFontSize(font_size.get());
	help_menu_box->setFontOutline(1);
	help_menu_box->addItem("Help 0");
	help_menu_box->addItem("Help 1");
	help_menu_box->addItem("Help 2");
	widget_menubar->setItemMenu(2, help_menu_box);

	help_menu_box->getEventClicked().connect(*this, [help_menu_box](){
		String msg = String("Menu: ") + help_menu_box->getCurrentItemText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void Menu::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_menubar);

		widget_menubar.deleteLater();
	}

	Console::setOnscreen(false);
}
