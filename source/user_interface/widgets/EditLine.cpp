#include "EditLine.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(EditLine);

using namespace Unigine;
using namespace Math;

void EditLine::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_editline = WidgetEditLine::create(gui, text.get());

	gui->addChild(widget_editline, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_editline->setPosition(position.get().x, position.get().y);
	widget_editline->setWidth(size.get().x);
	widget_editline->setHeight(size.get().y);
	widget_editline->setFontSize(font_size.get());

	widget_editline->getEventChanged().connect(*this, [this]() {
		String msg = String("EditLine text: ") + widget_editline->getText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void EditLine::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_editline);

		widget_editline.deleteLater();
	}

	Console::setOnscreen(false);
}
