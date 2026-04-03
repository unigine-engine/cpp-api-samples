// Demonstrates WidgetWindow as a container for child widgets. A resizable window
// is created with an edit line and button, each with event callbacks that log
// user interactions to the console.

#include "Window.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(Window);

using namespace Unigine;
using namespace Math;

// Resizable window is created with edit line and button, both with event handlers.
void Window::init()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	GuiPtr gui = main_window->getGui();

	// Create window with title and 4px padding
	window = WidgetWindow::create(gui, "Hello from C++", 4, 4);
	window->setWidth(320);
	// Enable window resizing by user
	window->setSizeable(1);

	// Edit line with placeholder text and change callback
	auto editline = WidgetEditLine::create(gui, "Edit me");
	window->addChild(editline, Gui::ALIGN_EXPAND);
	editline->getEventChanged().connect(this, &Window::editline_changed);
	editline->setFontSize(16);

	// Button with click callback
	auto button = WidgetButton::create(gui, "Press me");
	window->addChild(button, Gui::ALIGN_EXPAND);
	button->getEventClicked().connect(this, &Window::button_pressed);
	button->setFontSize(18);

	// Recalculate layout and display centered on screen
	window->arrange();
	main_window->addChild(window, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);

	// Enable on-screen console for log message visibility
	Console::setOnscreen(true);
}

// Window is released and on-screen console is disabled.
void Window::shutdown()
{
	window.deleteLater();

	Console::setOnscreen(false);
}

// Edit line text change is logged to demonstrate event callback.
void Window::editline_changed(const Unigine::WidgetPtr &widget)
{
	// Cast to WidgetEditLine to access getText method
	WidgetEditLinePtr editline = checked_ptr_cast<WidgetEditLine>(widget);
	Log::message("EditLine changed: %s\n", editline->getText());
}

// Button click is logged to demonstrate event callback.
void Window::button_pressed()
{
	Log::message("Button pressed\n");
}
