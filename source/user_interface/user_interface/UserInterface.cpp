// Demonstrates loading UI layouts from external .ui files via UserInterface class.
// Widgets are retrieved by name and connected to callbacks. The menubox includes
// a quit option that executes a console command to terminate the application.

#include "UserInterface.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(UserInterface);

// UI layout is loaded from file and widgets are connected to event handlers.
void UserInterface::init()
{
	if (Unigine::String::isEmpty(ui_file.get()))
	{
		Unigine::Log::warning("UserInterface::init(): ui_file is not assigned.\n");
		return;
	}

	Unigine::GuiPtr gui = Unigine::WindowManager::getMainWindow()->getGui();

	// Parse .ui XML file and create widget hierarchy
	user_interface = Unigine::UserInterface::create(gui, ui_file.get());
	if (user_interface == nullptr)
	{
		Unigine::Log::warning("UserInterface::init(): can't created UserInterface.\n");
	}

	// retrieve widgets by name from the loaded UI layout and connect event handlers
	Unigine::WidgetPtr edittext = user_interface->getWidgetByName("edittext");
	if (edittext != nullptr)
	{
		edittext->getEventChanged().connect(this, &UserInterface::edittext_changed);
	}

	Unigine::WidgetPtr menubox_0 = user_interface->getWidgetByName("menubox_0");
	if (menubox_0 != nullptr)
	{
		menubox_0->getEventClicked().connect(this, &UserInterface::menubox_0_clicked);
	}

	// findWidget returns an index that can be used with getWidget
	int index = user_interface->findWidget("window");
	if (index != -1)
	{
		window = user_interface->getWidget(index);
		// arrange() recalculates child widget positions before adding to GUI
		window->arrange();
		Unigine::WindowManager::getMainWindow()->addChild(window, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_CENTER);
	}

	Unigine::Console::setOnscreen(true);
}

// Window widget and UI layout are released.
void UserInterface::shutdown()
{
	if (window)
		window.deleteLater();

	if(user_interface)
		user_interface.deleteLater();

	Unigine::Console::setOnscreen(false);
}

// Text change event is logged to demonstrate callback binding.
void UserInterface::edittext_changed(const Unigine::WidgetPtr &widget)
{
	Unigine::WidgetEditTextPtr edittext = Unigine::checked_ptr_cast<Unigine::WidgetEditText>(widget);
	Unigine::Log::message("EditText changed: %s\n", edittext->getText());
}

// Menu selection is logged and quit command is executed for item index 2.
void UserInterface::menubox_0_clicked(const Unigine::WidgetPtr &widget)
{
	Unigine::WidgetMenuBoxPtr menubox = Unigine::checked_ptr_cast<Unigine::WidgetMenuBox>(widget);
	Unigine::Log::message("MenuBox clicked: %s\n", menubox->getCurrentItemText());
	// Item index 2 corresponds to the "Quit" option in the menu layout
	if (menubox->getCurrentItem() == 2)
		Unigine::Console::run("quit");
}
