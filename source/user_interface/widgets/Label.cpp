// Demonstrates WidgetLabel for displaying static text. The label is positioned
// and styled via component properties with no event handling required for this
// non-interactive widget type.

#include "Label.h"

REGISTER_COMPONENT(Label);

using namespace Unigine;
using namespace Math;

// Label widget is created with static text content.
void Label::init()
{
	gui = WindowManager::getMainWindow()->getGui();

	widget_label = WidgetLabel::create(gui, text.get());

	gui->addChild(widget_label, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	widget_label->setPosition(position.get().x, position.get().y);
	widget_label->setFontSize(font_size.get());
}

// Widget is removed from GUI and resources are released.
void Label::shutdown()
{
	if (gui)
	{
		gui->removeChild(widget_label);

		widget_label.deleteLater();
	}
}
