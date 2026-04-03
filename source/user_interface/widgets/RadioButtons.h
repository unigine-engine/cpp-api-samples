// Demonstrates radio button behavior using WidgetCheckBox with addAttach.
// Attaching checkboxes creates mutual exclusivity so only one can be selected
// at a time. Selection changes are displayed on the console overlay.

#pragma once
#include <UnigineComponentSystem.h>

// Creates mutually exclusive radio buttons using attached checkboxes.
class RadioButtons : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RadioButtons, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Screen position in pixels
	PROP_PARAM(Vec2, widget_position, Unigine::Math::vec2(600, 450));
	// Font size for radio button labels
	PROP_PARAM(Int, font_size, 16);
	// Horizontal padding inside the layout container
	PROP_PARAM(Int, horizontal_layout_space, 4);
	// Vertical spacing between radio buttons
	PROP_PARAM(Int, vertical_layout_space, 4);
	// Label text for the first radio button
	PROP_PARAM(String, first_rbutton_text, "Check Me");
	// Label text for the second radio button
	PROP_PARAM(String, second_rbutton_text, "Or Me");

private:
	void init();
	void shutdown();

	// Vertical layout container for arranging radio buttons
	Unigine::WidgetVBoxPtr p_vertical_layout;

	// First radio button checkbox widget
	Unigine::WidgetCheckBoxPtr p_first_rbutton = nullptr;
	// Second radio button checkbox widget
	Unigine::WidgetCheckBoxPtr p_second_rbutton = nullptr;
};
