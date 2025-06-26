#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineUserInterface.h>

class UserInterface : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UserInterface, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, ui_file);

private:
	void init();
	void shutdown();

	void edittext_changed(const Unigine::WidgetPtr &widget);
	void menubox_0_clicked(const Unigine::WidgetPtr &widget);

	Unigine::UserInterfacePtr user_interface;
	Unigine::WidgetPtr window;
};