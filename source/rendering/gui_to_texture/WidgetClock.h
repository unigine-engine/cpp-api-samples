// Real-time clock widget displayed via GUI-to-texture rendering.
// Uses manual GUI update (auto-update disabled) to only re-render
// when the displayed time actually changes, saving GPU resources.

#pragma once

#include <UnigineComponentSystem.h>

#include <ctime>

class GuiToTexture;

// Displays system time on a texture, updating only when seconds change.
class WidgetClock final : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(WidgetClock, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	void setCenterPosition(const Unigine::Math::ivec2 &pos);

private:
	void init();
	void update();

	void adjust_screen_position() const;
	void set_time(const std::time_t &time) const;

private:
	// Label widget showing HH:MM:SS format
	Unigine::WidgetLabelPtr widget_timer{};
	// Cached time to detect when update is needed
	std::time_t previous_time{};

	// Widget center position on the texture
	Unigine::Math::ivec2 position;

	// Reference to the GUI-to-texture renderer
	GuiToTexture *gui_to_texture{};
};
