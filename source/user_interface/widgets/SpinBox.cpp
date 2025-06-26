#include "SpinBox.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(SpinBox);

using namespace Unigine;
using namespace Math;

void SpinBox::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_editline = WidgetEditLine::create(gui, "0");
	widget_editline->setPosition(position.get().x, position.get().y);
	widget_editline->setFontOutline(1);

	gui->addChild(widget_editline, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_spinbox = WidgetSpinBox::create(gui);
	widget_spinbox->setOrder(widget_editline->getOrder() + 1);
	widget_editline->addAttach(widget_spinbox);

	gui->addChild(widget_spinbox, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_spinbox->getEventChanged().connect(*this, [this]() {
		String msg = String("SpinBox: ") + String::itoa(widget_spinbox->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void SpinBox::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_editline);
		gui->removeChild(widget_spinbox);

		widget_editline.deleteLater();
		widget_spinbox.deleteLater();
	}

	Console::setOnscreen(false);
}
