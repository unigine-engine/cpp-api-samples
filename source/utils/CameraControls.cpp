#include "CameraControls.h"

REGISTER_COMPONENT(CameraControls);

using namespace Unigine;
using namespace Unigine::Math;

void CameraControls::init()
{
	for (int i = 0; i < Input::getNumGamePads(); ++i)
	{
		auto game_pad = Input::getGamePad(i);
		if (game_pad->isAvailable())
		{
			gamepad = game_pad;
			break;
		}
	}
}

void CameraControls::update()
{
	forward = 0.0f;
	backward = 0.0f;
	right = 0.0f;
	left = 0.0f;
	up = 0.0f;
	down = 0.0f;
	turn_right = 0.0f;
	turn_left = 0.0f;
	turn_up = 0.0f;
	turn_down = 0.0f;
	acceleration = 1.0f;
	zoom_in = 0.0f;
	zoom_out = 0.0f;

	update_keyboard();
	update_mouse();
	update_gamepad();
}

void CameraControls::update_keyboard()
{
	if (!Input::isMouseGrab())
		return;

	if (Input::isKeyPressed((Input::KEY)forward_key.get()))
		forward = max(forward, 1.0f);

	if (Input::isKeyPressed((Input::KEY)backward_key.get()))
		backward = max(backward, 1.0f);

	if (Input::isKeyPressed((Input::KEY)right_key.get()))
		right = max(right, 1.0f);

	if (Input::isKeyPressed((Input::KEY)left_key.get()))
		left = max(left, 1.0f);

	if (Input::isKeyPressed((Input::KEY)up_key.get()))
		up = max(up, 1.0f);

	if (Input::isKeyPressed((Input::KEY)down_key.get()))
		down = max(down, 1.0f);

	if (Input::isKeyPressed((Input::KEY)acceleration_key.get()))
		acceleration = max(acceleration, 2.0f);
}

void CameraControls::update_mouse()
{
	if (!Input::isMouseGrab())
		return;

	auto delta = vec2(Input::getMouseDeltaPosition()) * 0.1f;

	if (delta.x > 0)
		turn_right = max(turn_right, delta.x * mouse_sensitivity);
	else
		turn_left = max(turn_left, -delta.x * mouse_sensitivity);

	if (delta.y > 0)
		turn_down = max(turn_up, delta.y * mouse_sensitivity);
	else
		turn_up = max(turn_down, -delta.y * mouse_sensitivity);

	if (Input::getMouseWheel() > 0)
		zoom_in = max(zoom_in, Input::getMouseWheel() * mouse_wheel_sensitivity);
	else
		zoom_out = max(zoom_out, -Input::getMouseWheel() * mouse_wheel_sensitivity);
}

void CameraControls::update_gamepad()
{
	using namespace Unigine::Math;

	if (!gamepad)
		return;

	right = max(get_gamepad_axis(gamepad_right_axis), right);
	left = max(get_gamepad_axis(gamepad_left_axis), left);

	forward = max(get_gamepad_axis(gamepad_forward_axis), forward);
	backward = max(get_gamepad_axis(gamepad_backward_axis), backward);

	up = max(get_gamepad_axis(gamepad_up_axis), up);
	down = max(get_gamepad_axis(gamepad_down_axis), down);

	turn_right = max(get_gamepad_axis(gamepad_turn_right_axis), turn_right);
	turn_left = max(get_gamepad_axis(gamepad_turn_left_axis), turn_left);

	turn_up = max(get_gamepad_axis(gamepad_turn_up_axis), turn_up);
	turn_down = max(get_gamepad_axis(gamepad_turn_down_axis), turn_down);

	if (gamepad->isButtonPressed((Input::GAMEPAD_BUTTON)gamepad_acceleration_button.get()))
		acceleration = max(acceleration, 2.0f);

	zoom_in = max(get_gamepad_axis(gamepad_zoom_in_axis), zoom_in);
	zoom_out = max(get_gamepad_axis(gamepad_zoom_out_axis), zoom_out);
}

float CameraControls::get_gamepad_axis(int axis)
{
	using namespace Unigine::Math;

	if (!gamepad)
		return 0.0f;

	float value = 0.0f;
	switch (axis)
	{
	case LEFT_AXIS_UP:
		value = clamp(gamepad->getAxesLeft().y, 0.0f, 1.0f);
		break;
	case LEFT_AXIS_DOWN:
		value = -clamp(gamepad->getAxesLeft().y, -1.0f, 0.0f);
		break;
	case LEFT_AXIS_RIGHT:
		value = clamp(gamepad->getAxesLeft().x, 0.0f, 1.0f);
		break;
	case LEFT_AXIS_LEFT:
		value = -clamp(gamepad->getAxesLeft().x, -1.0f, 0.0f);
		break;
	case RIGHT_AXIS_UP:
		value = clamp(gamepad->getAxesRight().y, 0.0f, 1.0f);
		break;
	case RIGHT_AXIS_DOWN:
		value = -clamp(gamepad->getAxesRight().y, -1.0f, 0.0f);
		break;
	case RIGHT_AXIS_RIGHT:
		value = clamp(gamepad->getAxesRight().x, 0.0f, 1.0f);
		break;
	case RIGHT_AXIS_LEFT:
		value = -clamp(gamepad->getAxesRight().x, -1.0f, 0.0f);
		break;
	case LEFT_TRIGGER:
		value = gamepad->getTriggerLeft();
		break;
	case RIGHT_TRIGGER:
		value = gamepad->getTriggerRight();
		break;
	default:
		break;
	}

	return max(gamepad_dead_zone, value);
}
