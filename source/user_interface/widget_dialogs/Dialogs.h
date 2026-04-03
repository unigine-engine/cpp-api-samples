// Demonstrates built-in dialog widgets for common UI interactions.
// Includes WidgetDialogMessage, WidgetDialogFile, WidgetDialogColor, and WidgetDialogImage.
// Each dialog handles OK/Cancel events with appropriate result logging.

#pragma once

#include <UnigineComponentSystem.h>

// Shows various dialog types with button-triggered display and event handling.
class Dialogs : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Dialogs, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Image file for WidgetDialogImage demonstration
	PROP_PARAM(File, image);

private:
	void init();
	void shutdown();

	// Handles OK button click with dialog type for result extraction
	void dialog_ok_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, Unigine::WidgetDialogPtr dialog, int type);
	// Handles Cancel button click to dismiss dialog
	void dialog_cancel_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, Unigine::WidgetDialogPtr dialog);
	// Displays dialog and connects OK/Cancel event handlers
	void dialog_show(const Unigine::WidgetDialogPtr &widget, int type);
	// Creates and shows message dialog
	void button_message_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *message);
	// Creates and shows file selection dialog
	void button_file_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *path);
	// Creates and shows color picker dialog
	void button_color_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, Unigine::Math::vec4 color);
	// Creates and shows image preview dialog
	void button_image_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *texture);

	// Container window holding dialog trigger buttons
	Unigine::WidgetWindowPtr window;
	// Currently active dialog instance
	Unigine::WidgetDialogPtr dialog;
};
