// Demonstrates multi-line text input using WidgetEditText. Text changes are
// monitored via EventChanged callback and displayed on the console overlay.
// Widget dimensions and font size are configured through component properties.

#include "EditText.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(EditText);

using namespace Unigine;
using namespace Math;

// Multi-line edit text widget is created and text change callback is connected.
void EditText::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_edittext = WidgetEditText::create(gui, text.get());

	gui->addChild(widget_edittext, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_edittext->setPosition(position.get().x, position.get().y);
	widget_edittext->setWidth(size.get().x);
	widget_edittext->setHeight(size.get().y);
	widget_edittext->setFontSize(font_size.get());

	// Lambda callback displays current text on each change
	widget_edittext->getEventChanged().connect(*this, [this]() {
		String msg = String("EditText text: ") + widget_edittext->getText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void EditText::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_edittext);

		widget_edittext.deleteLater();
	}

	Console::setOnscreen(false);
}
