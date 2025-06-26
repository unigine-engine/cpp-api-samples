#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class Window : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Window, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	void editline_changed(const Unigine::WidgetPtr &widget);
	void button_pressed();

	Unigine::WidgetWindowPtr window;
};