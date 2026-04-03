// Sample UI for triggering screenshot capture via button click.
// The screenshot is saved to data/screenshot.dds.

#include "ScreenshotSample.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ScreenshotSample);

// Screenshot helper is bound to main window; UI button triggers capture.
void ScreenshotSample::init()
{
	// Connect screenshot helper to the main application window
	screenshot.setWindow(WindowManager::getMainWindow());

	// Build sample UI with info label and capture button
	sample_description_window.createWindow();
	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	auto info_label = WidgetLabel::create("Screenshot will be saved in <b>data/screenshot.dds</b>");
	info_label->setFontRich(1);
	parameters->addChild(info_label, Gui::ALIGN_EXPAND);

	// Button click sets grab flag; actual capture happens at end of next frame
	auto screenshot_button = WidgetButton::create("Take Screenshot");
	parameters->addChild(screenshot_button, Gui::ALIGN_EXPAND);
	screenshot_button->getEventClicked().connect(*this, [this]() { screenshot.grab(); });
}

// UI window is cleaned up on component destruction.
void ScreenshotSample::shutdown()
{
	sample_description_window.shutdown();
}
