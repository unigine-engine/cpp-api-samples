// Demonstrates WidgetWindow as a container for child widgets. A resizable
// window is created with an edit line and button, each with event callbacks
// that log user interactions to the on-screen console.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Creates a simple window with edit line and button demonstrating event handling.
class Window : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Window, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	// Callback for edit line text change event
	void editline_changed(const Unigine::WidgetPtr &widget);
	// Callback for button click event
	void button_pressed();

	// Main container window holding child widgets
	Unigine::WidgetWindowPtr window;
};