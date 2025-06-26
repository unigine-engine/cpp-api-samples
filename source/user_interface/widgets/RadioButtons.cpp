#include "RadioButtons.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(RadioButtons)

using namespace Unigine;

void RadioButtons::init()
{
	auto gui = WindowManager::getMainWindow()->getGui();

	p_vertical_layout = WidgetVBox::create(horizontal_layout_space.get(), vertical_layout_space.get());

	gui->addChild(p_vertical_layout, Gui::ALIGN_OVERLAP);

	p_vertical_layout->setPosition(widget_position.get().x, widget_position.get().y);
	p_vertical_layout->setBackground(1);

	p_first_rbutton = WidgetCheckBox::create(first_rbutton_text.get());
	p_first_rbutton->setChecked(true); // Set the first checkbox as selected by default

	p_vertical_layout->addChild(p_first_rbutton, Gui::ALIGN_LEFT);

	p_first_rbutton->setFontSize(font_size.get());

	p_first_rbutton->getEventChanged().connect(*this, [this]() {
		if (p_first_rbutton->isChecked())
			Console::onscreenMessageLine("Radio buttons: first option");
	});

	p_second_rbutton = WidgetCheckBox::create(second_rbutton_text.get());

	p_vertical_layout->addChild(p_second_rbutton, Gui::ALIGN_LEFT);
	p_first_rbutton->addAttach(p_second_rbutton); // Attach the second checkbox to the first to group them as radio buttons

	p_second_rbutton->setFontSize(font_size.get());

	p_second_rbutton->getEventChanged().connect(*this, [this]() {
		if (p_second_rbutton->isChecked())
			Console::onscreenMessageLine("Radio buttons: second option");
	});


	Console::setOnscreen(true);
}

void RadioButtons::shutdown()
{
	p_vertical_layout.deleteLater();
	Console::setOnscreen(false);
}
