#include "UserInterface.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(UserInterface);

void UserInterface::init()
{
	if (Unigine::String::isEmpty(ui_file.get()))
	{
		Unigine::Log::warning("UserInterface::init(): ui_file is not assigned.\n");
		return;
	}

	Unigine::GuiPtr gui = Unigine::WindowManager::getMainWindow()->getGui();

	user_interface = Unigine::UserInterface::create(gui, ui_file.get());
	if (user_interface == nullptr)
	{
		Unigine::Log::warning("UserInterface::init(): can't created UserInterface.\n");
	}

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

	int index = user_interface->findWidget("window");
	if (index != -1)
	{
		window = user_interface->getWidget(index);
		window->arrange();
		Unigine::WindowManager::getMainWindow()->addChild(window, Unigine::Gui::ALIGN_OVERLAP | Unigine::Gui::ALIGN_CENTER);
	}

	Unigine::Console::setOnscreen(true);
}

void UserInterface::shutdown()
{	
	if (window)
		window.deleteLater();

	if(user_interface)
		user_interface.deleteLater();

	Unigine::Console::setOnscreen(false);
}

void UserInterface::edittext_changed(const Unigine::WidgetPtr &widget)
{
	Unigine::WidgetEditTextPtr edittext = Unigine::checked_ptr_cast<Unigine::WidgetEditText>(widget);
	Unigine::Log::message("EditText changed: %s\n", edittext->getText());
}

void UserInterface::menubox_0_clicked(const Unigine::WidgetPtr &widget)
{
	Unigine::WidgetMenuBoxPtr menubox = Unigine::checked_ptr_cast<Unigine::WidgetMenuBox>(widget);
	Unigine::Log::message("MenuBox clicked: %s\n", menubox->getCurrentItemText());
	if (menubox->getCurrentItem() == 2)
		Unigine::Console::run("quit");
}
