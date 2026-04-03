// Loads GUI layouts from external .ui XML files using UserInterface class.
// Widgets are retrieved by name for event binding. Demonstrates dynamic
// UI loading without hardcoding widget creation in code.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineUserInterface.h>

// Manages UI layout loaded from external file with widget event callbacks.
class UserInterface : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UserInterface, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Path to .ui XML file containing widget layout definition
	PROP_PARAM(File, ui_file);

private:
	void init();
	void shutdown();

	// Callback invoked when edit text content changes
	void edittext_changed(const Unigine::WidgetPtr &widget);
	// Callback invoked when menubox item is clicked
	void menubox_0_clicked(const Unigine::WidgetPtr &widget);

	// Loaded UI layout containing widget definitions
	Unigine::UserInterfacePtr user_interface;
	// Main window widget from loaded layout
	Unigine::WidgetPtr window;
};