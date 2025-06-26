#pragma once

#include <UnigineComponentSystem.h>

#include <ctime>

class GuiToTexture;

/// This sample demonstrates how the GuiToTexture component can be used to render custom widgets
/// In this component we're going to use GuiToTexture with auto update disabled and will update gui manually in set_time method
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
	Unigine::WidgetLabelPtr widget_timer{};
	std::time_t previous_time{};

	Unigine::Math::ivec2 position;

	GuiToTexture *gui_to_texture{};
};
