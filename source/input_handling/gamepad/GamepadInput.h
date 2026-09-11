#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>
#include <UnigineWidgets.h>

class Car;

// Demonstrates gamepad input handling with button states, analog axes, and triggers.
// Reads left/right stick movements and trigger values, maps them to car controls,
// and supports vibration feedback with configurable low/high frequency motors.
// Touch input visualization is also included for gamepads with touchpad support.
class GamepadInput : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(GamepadInput, Unigine::ComponentBase);

	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	void setFilter(float f);
	void setLowFrequency(float f);
	void setHighFrequency(float f);
	void setDuration(float d);

private:
	void init();
	void update();
	void shutdown();

	void update_inputs();

	void draw_touches();

	Unigine::String getDeviceName(Unigine::Input::DEVICE t);
	Unigine::String getModelName(Unigine::InputGamePad::MODEL_TYPE t);
	Unigine::String getGamePadButtonName(Unigine::Input::GAMEPAD_BUTTON btn);

	Unigine::InputGamePadPtr gamepad;

	Car* car = nullptr;
	SampleDescriptionWindow description_window;

	Unigine::WidgetGroupBoxPtr info_group;
	Unigine::WidgetLabelPtr info_label;
	Unigine::WidgetGroupBoxPtr canvas_group;
	Unigine::WidgetCanvasPtr canvas;

	Unigine::String last_button_down = "";
	Unigine::String last_button_pressed = "";
	Unigine::String last_button_up = "";

	Unigine::HashSet<int> pressed_buttons;

	Unigine::Math::vec2 last_axes_left_delta{ 0.0f, 0.0f };
	Unigine::Math::vec2 last_axes_right_delta{ 0.0f, 0.0f };

	float last_trigger_left_delta = 0.0f;
	float last_trigger_right_delta = 0.0f;

	float low_frequency = 0.0f;
	float high_frequency = 0.0f;
	float vibration_duration = 0.0f;
};
