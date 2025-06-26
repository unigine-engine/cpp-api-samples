#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInput.h>

class Car;
class SimpleInformationBox;

class GamepadInput : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(GamepadInput, Unigine::ComponentBase);

	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);

	void setFilter(float f);
	void setLowFrequency(float f);
	void setHighFrequency(float f);
	void setDuration(float d);

private:
	void init();
	void update();

	void update_inputs();

	void draw_touches();

	Unigine::String getDeviceName(Unigine::Input::DEVICE t);
	Unigine::String getModelName(Unigine::InputGamePad::MODEL_TYPE t);
	Unigine::String getGamePadButtonName(Unigine::Input::GAMEPAD_BUTTON btn);

	Unigine::InputGamePadPtr gamepad;

	Car* car = nullptr;
	SimpleInformationBox* info = nullptr;

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
	Unigine::WidgetCanvasPtr canvas;

	Unigine::EventConnections widget_connections;
};
