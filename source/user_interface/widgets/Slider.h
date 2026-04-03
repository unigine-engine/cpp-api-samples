// Demonstrates WidgetSlider for value selection via draggable handle. Slider
// dimensions and button width are configured via component properties, with
// value changes shown on the console overlay via EventChanged callback.

#pragma once
#include <UnigineComponentSystem.h>

// Creates a draggable slider widget for value selection.
class Slider : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Slider, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, position, Unigine::Math::vec2(600, 150));
	// Widget dimensions in pixels
	PROP_PARAM(Vec2, size, Unigine::Math::vec2(100, 50));
	// Width of the slider handle button
	PROP_PARAM(Int, button_width, 30)

private:
	void init();
	void shutdown();

	// GUI context for widget creation
	Unigine::GuiPtr gui;
	// Slider widget instance
	Unigine::WidgetSliderPtr widget_slider;
};
