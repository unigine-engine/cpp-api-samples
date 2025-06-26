#pragma once

#include <UnigineComponentSystem.h>

class Dialogs : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Dialogs, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, image);

private:
	void init();
	void shutdown();

	void dialog_ok_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, Unigine::WidgetDialogPtr dialog, int type);
	void dialog_cancel_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, Unigine::WidgetDialogPtr dialog);
	void dialog_show(const Unigine::WidgetDialogPtr &widget, int type);
	void button_message_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *message);
	void button_file_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *path);
	void button_color_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, Unigine::Math::vec4 color);
	void button_image_clicked(const Unigine::WidgetPtr &widget, int mouse_buttons, const char *title, const char *texture);
	
	Unigine::WidgetWindowPtr window;
	Unigine::WidgetDialogPtr dialog;
};
