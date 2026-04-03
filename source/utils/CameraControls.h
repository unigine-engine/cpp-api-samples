#pragma once
#include "StringEnums.h"
#include <UnigineComponentSystem.h>

class CameraControls : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CameraControls, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	enum GAMEPAD_AXES
	{
		LEFT_AXIS_UP,
		LEFT_AXIS_DOWN,
		LEFT_AXIS_RIGHT,
		LEFT_AXIS_LEFT,
		RIGHT_AXIS_UP,
		RIGHT_AXIS_DOWN,
		RIGHT_AXIS_RIGHT,
		RIGHT_AXIS_LEFT,
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		NONE
	};

	PROP_PARAM(Float, mouse_sensitivity, 1.25f);
	PROP_PARAM(Float, mouse_wheel_sensitivity, 10.0f);
	PROP_PARAM(Switch, forward_key, Unigine::Input::KEY_W, KEYS_SWITCH);
	PROP_PARAM(Switch, backward_key, Unigine::Input::KEY_S, KEYS_SWITCH);
	PROP_PARAM(Switch, right_key, Unigine::Input::KEY_D, KEYS_SWITCH);
	PROP_PARAM(Switch, left_key, Unigine::Input::KEY_A, KEYS_SWITCH);
	PROP_PARAM(Switch, up_key, Unigine::Input::KEY_E, KEYS_SWITCH);
	PROP_PARAM(Switch, down_key, Unigine::Input::KEY_Q, KEYS_SWITCH);
	PROP_PARAM(Switch, acceleration_key, Unigine::Input::KEY_ANY_SHIFT, KEYS_SWITCH);

	PROP_PARAM(Float, gamepad_dead_zone, 0.2f);

	PROP_PARAM(Switch, gamepad_acceleration_button, Unigine::Input::GAMEPAD_BUTTON_SHOULDER_RIGHT, GAMEPAD_BUTTONS_SWITCH);
	PROP_PARAM(Switch, gamepad_forward_axis, GAMEPAD_AXES::LEFT_AXIS_UP, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_backward_axis, GAMEPAD_AXES::LEFT_AXIS_DOWN, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_right_axis, GAMEPAD_AXES::LEFT_AXIS_RIGHT, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_left_axis, GAMEPAD_AXES::LEFT_AXIS_LEFT, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_up_axis, GAMEPAD_AXES::RIGHT_TRIGGER, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_down_axis, GAMEPAD_AXES::LEFT_TRIGGER, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_turn_right_axis, GAMEPAD_AXES::RIGHT_AXIS_RIGHT, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_turn_left_axis, GAMEPAD_AXES::RIGHT_AXIS_LEFT, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_turn_up_axis, GAMEPAD_AXES::RIGHT_AXIS_UP, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_turn_down_axis, GAMEPAD_AXES::RIGHT_AXIS_DOWN, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_zoom_in_axis, GAMEPAD_AXES::RIGHT_TRIGGER, GAMEPAD_AXES_SWITCH);
	PROP_PARAM(Switch, gamepad_zoom_out_axis, GAMEPAD_AXES::LEFT_TRIGGER, GAMEPAD_AXES_SWITCH);

	UNIGINE_INLINE float getForward() const noexcept { return forward; }
	UNIGINE_INLINE float getBackward() const noexcept { return backward; }
	UNIGINE_INLINE float getRight() const noexcept { return right; }
	UNIGINE_INLINE float getLeft() const noexcept { return left; }
	UNIGINE_INLINE float getUp() const noexcept { return up; }
	UNIGINE_INLINE float getDown() const noexcept { return down; }
	UNIGINE_INLINE float getTurnRight() const noexcept { return turn_right; }
	UNIGINE_INLINE float getTurnLeft() const noexcept { return turn_left; }
	UNIGINE_INLINE float getTurnUp() const noexcept { return turn_up; }
	UNIGINE_INLINE float getTurnDown() const noexcept { return turn_down; }
	UNIGINE_INLINE float getAcceleration() const noexcept { return acceleration; }
	UNIGINE_INLINE float getZoomIn() const noexcept { return zoom_in; }
	UNIGINE_INLINE float getZoomOut() const noexcept { return zoom_out; }

private:
	void init();
	void update();

private:
	void update_keyboard();
	void update_mouse();
	void update_gamepad();
	float get_gamepad_axis(int axis);

private:
	Unigine::InputGamePadPtr gamepad = nullptr;

	float forward = 0.0f;
	float backward = 0.0f;
	float right = 0.0f;
	float left = 0.0f;
	float up = 0.0f;
	float down = 0.0f;
	float turn_right = 0.0f;
	float turn_left = 0.0f;
	float turn_up = 0.0f;
	float turn_down = 0.0f;
	float acceleration = 1.0f;
	float zoom_in = 0.0f;
	float zoom_out = 0.0f;
};
