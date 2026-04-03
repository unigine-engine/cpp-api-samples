// Real-time clock widget rendered to texture. Updates only when seconds
// change to avoid unnecessary re-rendering. Demonstrates manual GUI update
// control with GuiToTexture auto-update disabled.

#include "WidgetClock.h"

#include "GuiToTexture.h"

REGISTER_COMPONENT(WidgetClock);

using namespace Unigine;

// Stores new position and immediately repositions the widget.
void WidgetClock::setCenterPosition(const Math::ivec2 &pos)
{
	position = pos;
	adjust_screen_position();
}

// Timer widget is created and centered; auto-update is disabled for manual control.
void WidgetClock::init()
{
	gui_to_texture = ComponentSystem::get()->getComponent<GuiToTexture>(node);
	// Auto-update is disabled since we only need to re-render when time changes
	gui_to_texture->setAutoUpdateEnabled(false);

	// Get our custom GUI
	const GuiPtr gui = gui_to_texture->getGui();


	widget_timer = WidgetLabel::create(gui);
	widget_timer->setFontSize(150);

	// Add widget as a child in GUI
	gui->addChild(widget_timer, Gui::ALIGN_OVERLAP);

	setCenterPosition(gui_to_texture->getTextureResolution() / 2);

	previous_time = std::time(nullptr);
	// Set time and update GUI
	set_time(previous_time);
}

// Time is checked each frame; texture is only re-rendered when seconds change.
void WidgetClock::update()
{
	auto clock_equal = [](std::time_t left, std::time_t right) {
		return Math::abs(difftime(left, right)) < 1.;
	};

	const std::time_t now = std::time(nullptr);
	if (clock_equal(now, previous_time))
	{
		return;
	}

	set_time(now);
	previous_time = now;
}

// Widget is repositioned to keep text centered at the target position.
void WidgetClock::adjust_screen_position() const
{
	Math::ivec2 widget_size;
	widget_size.y = widget_timer->getTextRenderSize(widget_timer->getText()).y;
	widget_size.x = widget_timer->getTextRenderSize(widget_timer->getText()).x;
	widget_timer->setPositionX(position.x - widget_size.x / 2);
	widget_timer->setPositionY(position.y - widget_size.y / 2);
}

// Label text is formatted and texture is re-rendered with updated time.
void WidgetClock::set_time(const std::time_t &time) const
{
	tm tstruct = *localtime(&time);
	widget_timer->setText(String::format("%02d:%02d:%02d", tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec));
	adjust_screen_position();
	gui_to_texture->renderToTexture();
}