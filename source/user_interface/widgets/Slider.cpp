// Demonstrates WidgetSlider for value selection via draggable handle. Slider
// dimensions and button width are configured via component properties, with
// value changes shown on the console overlay via EventChanged callback.

#include "Slider.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(Slider);

using namespace Unigine;
using namespace Math;

// Slider widget is created with configured dimensions and value change callback.
void Slider::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_slider = WidgetSlider::create(gui);

	gui->addChild(widget_slider, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_slider->setPosition(position.get().x, position.get().y);
	widget_slider->setWidth(size.get().x);
	widget_slider->setHeight(size.get().y);
	widget_slider->setButtonWidth(button_width.get());

	// Lambda callback displays current slider value on change
	widget_slider->getEventChanged().connect(*this, [this]() {
		String msg = String("Slider: ") + String::itoa(widget_slider->getValue());
		Console::onscreenMessageLine(msg.get());
		});

	Console::setOnscreen(true);
}

// Widget is removed from GUI and resources are released.
void Slider::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_slider);

		widget_slider.deleteLater();
	}

	Console::setOnscreen(false);
}
