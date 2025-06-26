#include "Window.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(Window);

using namespace Unigine;
using namespace Math;

void Window::init()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	GuiPtr gui = main_window->getGui();

	window = WidgetWindow::create(gui, "Hello from C++", 4, 4);
	window->setWidth(320);
	window->setSizeable(1);

	auto editline = WidgetEditLine::create(gui, "Edit me");
	window->addChild(editline, Gui::ALIGN_EXPAND);
	editline->getEventChanged().connect(this, &Window::editline_changed);
	editline->setFontSize(16);

	auto button = WidgetButton::create(gui, "Press me");
	window->addChild(button, Gui::ALIGN_EXPAND);
	button->getEventClicked().connect(this, &Window::button_pressed);
	button->setFontSize(18);

	window->arrange();
	main_window->addChild(window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);

	Console::setOnscreen(true);
}

void Window::shutdown()
{
	window.deleteLater();

	Console::setOnscreen(false);
}

void Window::editline_changed(const Unigine::WidgetPtr &widget)
{
	WidgetEditLinePtr editline = checked_ptr_cast<WidgetEditLine>(widget);
	Log::message("EditLine changed: %s\n", editline->getText());
}

void Window::button_pressed()
{
	Log::message("Button pressed\n");
}
