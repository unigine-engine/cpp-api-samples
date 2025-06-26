#include "Slider.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Slider);

using namespace Unigine;
using namespace Math;

void Slider::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_slider = WidgetSlider::create(gui);

	gui->addChild(widget_slider, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_slider->setPosition(position.get().x, position.get().y);
	widget_slider->setWidth(size.get().x);
	widget_slider->setHeight(size.get().y);
	widget_slider->setButtonWidth(button_width.get());

	widget_slider->getEventChanged().connect(*this, [this]() {
		String msg = String("Slider: ") + String::itoa(widget_slider->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

void Slider::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_slider);

		widget_slider.deleteLater();
	}

	Console::setOnscreen(false);
}
