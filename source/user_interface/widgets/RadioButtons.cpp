// Demonstrates radio button behavior using WidgetCheckBox with addAttach.
// Attaching checkboxes creates mutual exclusivity so only one can be selected
// at a time. Selection changes are displayed on the console overlay.

#include "RadioButtons.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(RadioButtons)

using namespace Unigine;

// Radio buttons are created as attached checkboxes in a vertical layout with change callbacks.
void RadioButtons::init()
{
	auto gui = WindowManager::getMainWindow()->getGui();

	p_vertical_layout = WidgetVBox::create(horizontal_layout_space.get(), vertical_layout_space.get());

	gui->addChild(p_vertical_layout, Gui::ALIGN_OVERLAP);

	p_vertical_layout->setPosition(widget_position.get().x, widget_position.get().y);
	p_vertical_layout->setBackground(1);

	// Create first radio button and set it as initially selected
	p_first_rbutton = WidgetCheckBox::create(first_rbutton_text.get());
	p_first_rbutton->setChecked(true);

	p_vertical_layout->addChild(p_first_rbutton, Gui::ALIGN_LEFT);

	p_first_rbutton->setFontSize(font_size.get());

	// Lambda callback displays selection message for first option
	p_first_rbutton->getEventChanged().connect(*this, [this]() {
		if (p_first_rbutton->isChecked())
			Console::onscreenMessageLine("Radio buttons: first option");
	});

	// Create second radio button
	p_second_rbutton = WidgetCheckBox::create(second_rbutton_text.get());

	p_vertical_layout->addChild(p_second_rbutton, Gui::ALIGN_LEFT);
	// Attach second checkbox to first for mutual exclusivity
	p_first_rbutton->addAttach(p_second_rbutton);

	p_second_rbutton->setFontSize(font_size.get());

	// Lambda callback displays selection message for second option
	p_second_rbutton->getEventChanged().connect(*this, [this]() {
		if (p_second_rbutton->isChecked())
			Console::onscreenMessageLine("Radio buttons: second option");
	});


	Console::setOnscreen(true);
}

// Layout container and child widgets are released.
void RadioButtons::shutdown()
{
	p_vertical_layout.deleteLater();
	Console::setOnscreen(false);
}
