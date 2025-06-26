#include "Dialogs.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(Dialogs);

using namespace Unigine;
using namespace Math;

void Dialogs::init()
{
	EngineWindowViewportPtr main_window = WindowManager::getMainWindow();

	GuiPtr gui = main_window->getGui();

	window = WidgetWindow::create(gui, "Dialogs", 4, 4);
	window->setFlags(Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);

	auto button0 = WidgetButton::create(gui, "Message");
	button0->getEventClicked().connect(this, &Dialogs::button_message_clicked, "DialogMessage", "Message");
	window->addChild(button0, Gui::ALIGN_EXPAND);

	auto button1 = WidgetButton::create(gui, "File");
	button1->getEventClicked().connect(this, &Dialogs::button_file_clicked, "DialogFile", "./");
	window->addChild(button1, Gui::ALIGN_EXPAND);

	auto button2 = WidgetButton::create(gui, "Color");
	button2->getEventClicked().connect(this, &Dialogs::button_color_clicked, "DialogColor", vec4(1.0f));
	window->addChild(button2, Gui::ALIGN_EXPAND);

	if (!String::isEmpty(image.get()))
	{
		auto button3 = WidgetButton::create(gui, "Image");
		button3->getEventClicked().connect(this, &Dialogs::button_image_clicked, "Dialogimage", image.get());
		window->addChild(button3, Gui::ALIGN_EXPAND);
	}
	else
	{
		Log::warning("Dialogs::init(): image parameter not specified.");
	}

	window->arrange();
	main_window->addChild(window);

	Console::setOnscreen(true);
}

void Dialogs::shutdown()
{
	window.deleteLater();

	if (dialog)
		dialog.deleteLater();

	Console::setOnscreen(false);
}

void Dialogs::dialog_ok_clicked(const WidgetPtr &widget, int mouse_buttons, WidgetDialogPtr dialog, int type)
{
	Log::message("%s ok clicked\n", dialog->getText());
	if (type == 1)
		Log::message("%s\n", static_ptr_cast<WidgetDialogFile>(dialog)->getFile());
	if (type == 2)
		Log::message("%s\n", static_ptr_cast<WidgetDialogColor>(dialog)->getWebColor());
	dialog.deleteLater();
}

void Dialogs::dialog_cancel_clicked(const WidgetPtr &widget, int mouse_buttons, WidgetDialogPtr dialog)
{
	Log::message("%s cancel clicked\n", dialog->getText());
	dialog.deleteLater();
}

void Dialogs::dialog_show(const WidgetDialogPtr &widget, int type)
{
	if (dialog)
		dialog.deleteLater();

	dialog = widget;

	dialog->getOkButton()->getEventClicked().connect(this, &Dialogs::dialog_ok_clicked, dialog, type);
	dialog->getCancelButton()->getEventClicked().connect(this, &Dialogs::dialog_cancel_clicked, dialog);
	WindowManager::getMainWindow()->addChild(dialog, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);
	dialog->setPermanentFocus();
}

void Dialogs::button_message_clicked(const WidgetPtr &widget, int mouse_buttons, const char *title, const char *message)
{
	GuiPtr gui = WindowManager::getMainWindow()->getGui();

	auto dialog_message = WidgetDialogMessage::create(gui, title);
	dialog_message->setMessageText(message);
	dialog_show(dialog_message, 0);
}

void Dialogs::button_file_clicked(const WidgetPtr &widget, int mouse_buttons, const char *title, const char *path)
{
	GuiPtr gui = WindowManager::getMainWindow()->getGui();

	auto dialog_file = WidgetDialogFile::create(gui, title);
	dialog_file->setPath(path);
	dialog_show(dialog_file, 1);
}

void Dialogs::button_color_clicked(const WidgetPtr &widget, int mouse_buttons, const char *title, vec4 color)
{
	GuiPtr gui = WindowManager::getMainWindow()->getGui();

	auto dialog_color = WidgetDialogColor::create(gui, title);
	dialog_color->setColor(color);
	dialog_show(dialog_color, 2);
}

void Dialogs::button_image_clicked(const WidgetPtr &widget, int mouse_buttons, const char *title, const char *texture)
{
	GuiPtr gui = WindowManager::getMainWindow()->getGui();

	auto dialog_image = WidgetDialogImage::create(gui, title);
	dialog_image->setTexture(texture);
	dialog_show(dialog_image, 3);
}
