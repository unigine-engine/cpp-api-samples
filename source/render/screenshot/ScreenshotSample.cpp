#include "ScreenshotSample.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ScreenshotSample);

void ScreenshotSample::init()
{
	screenshot.setWindow(WindowManager::getMainWindow());

	sample_description_window.createWindow();
	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	auto info_label = WidgetLabel::create("Screenshot will be saved in <b>data/screenshot.dds</b>");
	info_label->setFontRich(1);
	parameters->addChild(info_label, Gui::ALIGN_EXPAND);

	auto screenshot_button = WidgetButton::create("Take Screenshot");
	parameters->addChild(screenshot_button, Gui::ALIGN_EXPAND);
	screenshot_button->getEventClicked().connect(*this, [this]() { screenshot.grab(); });
}

void ScreenshotSample::shutdown()
{
	sample_description_window.shutdown();
}
