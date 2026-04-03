// Demonstrates single-line text input using WidgetEditLine. Text changes are
// monitored via EventChanged callback and displayed on the console overlay.
// Widget size and font are configured through component properties.

#include "EditLine.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(EditLine);

using namespace Unigine;
using namespace Math;

// Edit line widget is created and text change callback is connected.
void EditLine::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_editline = WidgetEditLine::create(gui, text.get());

	gui->addChild(widget_editline, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_editline->setPosition(position.get().x, position.get().y);
	widget_editline->setWidth(size.get().x);
	widget_editline->setHeight(size.get().y);
	widget_editline->setFontSize(font_size.get());

	// Lambda callback displays current text on each change
	widget_editline->getEventChanged().connect(*this, [this]() {
		String msg = String("EditLine text: ") + widget_editline->getText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void EditLine::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_editline);

		widget_editline.deleteLater();
	}

	Console::setOnscreen(false);
}
