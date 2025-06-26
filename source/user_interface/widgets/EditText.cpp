#include "EditText.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(EditText);

using namespace Unigine;
using namespace Math;

void EditText::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_edittext = WidgetEditText::create(gui, text.get());

	gui->addChild(widget_edittext, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_edittext->setPosition(position.get().x, position.get().y);
	widget_edittext->setWidth(size.get().x);
	widget_edittext->setHeight(size.get().y);
	widget_edittext->setFontSize(font_size.get());

	widget_edittext->getEventChanged().connect(*this, [this]() {
		String msg = String("EditText text: ") + widget_edittext->getText();
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void EditText::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_edittext);

		widget_edittext.deleteLater();
	}

	Console::setOnscreen(false);
}
