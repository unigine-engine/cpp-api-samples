#include "CheckBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(CheckBox);

using namespace Unigine;
using namespace Math;

void CheckBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_checkbox = WidgetCheckBox::create(gui, text.get());

	gui->addChild(widget_checkbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_checkbox->setPosition(position.get().x, position.get().y);
	widget_checkbox->setFontSize(font_size.get());

	widget_checkbox->getEventChanged().connect(*this, [this]() {
		String msg = String("CheckBox: ") + (widget_checkbox->isChecked() ? "True" : "False");
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void CheckBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_checkbox);

		widget_checkbox.deleteLater();
	}

	Console::setOnscreen(false);
}
