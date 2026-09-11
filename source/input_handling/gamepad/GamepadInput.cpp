// Gamepad input sample that controls a car using analog sticks and triggers. Shows
// button state tracking (down/pressed/up), axis values, trigger inputs, and touchpad
// visualization. Supports force feedback vibration with configurable parameters.

#include "GamepadInput.h"
#include "Car.h"
#include <UnigineGame.h>
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(GamepadInput);

using namespace Unigine;
using namespace Math;

// Input filter threshold is applied to the gamepad.
void GamepadInput::setFilter(float f)
{
	if (gamepad)
	{
		gamepad->setFilter(f);
	}
}

// Low frequency vibration motor power is set (0.0 to 1.0).
void GamepadInput::setLowFrequency(float f)
{
	low_frequency = clamp(f, 0.0f, 1.0f);
}

// High frequency vibration motor power is set (0.0 to 1.0).
void GamepadInput::setHighFrequency(float f)
{
	high_frequency = clamp(f, 0.0f, 1.0f);
}

// Vibration duration is set in milliseconds.
void GamepadInput::setDuration(float d)
{
	vibration_duration = d;
	if (vibration_duration < 0.0f)
		vibration_duration = 0.0f;
}


// Physics settings are configured, UI is created, and gamepad is acquired.
void GamepadInput::init()
{
	// Physics freeze threshold is lowered to keep the car responsive
	Physics::setFrozenLinearVelocity(0.1f);
	Physics::setFrozenAngularVelocity(0.1f);

	description_window.createWindow();

	description_window.addFloatParameter("Filter", "Buttons sensitivity threshold",
		0.0f, 0.0f, 1.0f,
		[this](float v) { setFilter(v); });
	description_window.addFloatParameter("Low Frequency", "Power of vibration for low frequency",
		0.0f, 0.0f, 1.0f,
		[this](float v) { setLowFrequency(v); });
	description_window.addFloatParameter("High Frequency", "Power of vibration for high frequency",
		0.0f, 0.0f, 1.0f,
		[this](float v) { setHighFrequency(v); });
	description_window.addIntParameter("Duration", "Vibration duration in ms",
		1, 1, 1000,
		[this](int v) { setDuration((float)v); });

	canvas_group = WidgetGroupBox::create("Touchpad", 8, 8);
	description_window.getWindow()->addChild(canvas_group, Gui::ALIGN_LEFT);
	canvas = WidgetCanvas::create();
	canvas->setWidth(280);
	canvas->setHeight(140);
	canvas_group->addChild(canvas, Gui::ALIGN_EXPAND);
	canvas_group->setHidden(true);

	info_group = WidgetGroupBox::create("Gamepad Info", 8, 8);
	description_window.getWindow()->addChild(info_group, Gui::ALIGN_LEFT);
	info_label = WidgetLabel::create();
	info_label->setFontRich(1);
	info_label->setFontWrap(1);
	info_group->addChild(info_label, Gui::ALIGN_EXPAND);

	car = getComponent<Car>(World::getNodeByName("car"));

	// First available gamepad is acquired
	if (Input::getNumGamePads() > 0)
	{
		gamepad = Input::getGamePad(0);
	}
	else
	{
		WindowManager::dialogError("Warning", " No available gamepads!");
		return;
	}
}

// Gamepad state is read, UI is updated, and car controls are applied.
void GamepadInput::update()
{
	update_inputs();

	if (gamepad && gamepad->isAvailable())
	{
		description_window.getParameterGroupBox()->setHidden(false);

		draw_touches();

		String text = String::format(
			"<center><b>%s</b></center>\n"
			"\n"
			"<center>Press Left and Right triggers to run motors</center>\n"
			"<center>Press <b>X button</b> to respawn</center>\n"
			"<center>Press <b>Y button</b> to vibrate</center>\n"
			"\n"
			"Number: %d\n"
			"Player Index: %d\n"
			"Device Type: %s\n"
			"Model Type: %s\n"
			"\n"
			"<center><b>Axes</b></center>\n"
			"Left X: %s\n"
			"Left Y: %s\n"
			"Right X: %s\n"
			"Right Y: %s\n"
			"Left X Last Delta: %s\n"
			"Left Y Last Delta: %s\n"
			"Right X Last Delta: %s\n"
			"Right Y Last Delta: %s\n"
			"\n"
			"<center><b>Triggers</b></center>\n"
			"Left: %s\n"
			"Right: %s\n"
			"Left Last Delta: %s\n"
			"Right Last Delta: %s\n"
			"\n"
			"<center><b>Buttons</b></center>\n"
			"Last Button Down: %s\n"
			"Last Button Pressed: %s\n"
			"Last Button Up: %s",
			gamepad->getName(),
			gamepad->getNumber(),
			gamepad->getPlayerIndex(),
			getDeviceName(gamepad->getDeviceType()).get(),
			getModelName(gamepad->getModelType()).get(),
			String::ftoa(gamepad->getAxesLeft().x).get(),
			String::ftoa(gamepad->getAxesLeft().y).get(),
			String::ftoa(gamepad->getAxesRight().x).get(),
			String::ftoa(gamepad->getAxesRight().y).get(),
			String::ftoa(last_axes_left_delta.x).get(),
			String::ftoa(last_axes_left_delta.y).get(),
			String::ftoa(last_axes_right_delta.x).get(),
			String::ftoa(last_axes_right_delta.y).get(),
			String::ftoa(gamepad->getTriggerLeft()).get(),
			String::ftoa(gamepad->getTriggerRight()).get(),
			String::ftoa(last_trigger_left_delta).get(),
			String::ftoa(last_trigger_right_delta).get(),
			last_button_down.get(),
			last_button_pressed.get(),
			last_button_up.get());

		info_label->setText(text.get());

		if (gamepad->isButtonPressed(Input::GAMEPAD_BUTTON_X))
			car->respawn();

		if (gamepad->isButtonDown(Input::GAMEPAD_BUTTON_Y))
			gamepad->setVibration(low_frequency, high_frequency, vibration_duration);

		car->setAngle(gamepad->getAxesLeft().x);
		car->setVelocity(gamepad->getTriggerRight());
		car->setBrake(gamepad->getTriggerLeft());
	}
	else
	{
		info_label->setText("<center>Connect gamepad.</center>");
		canvas_group->setHidden(true);
		description_window.getParameterGroupBox()->setHidden(true);

		if (!gamepad && Input::getNumGamePads())
			gamepad = Input::getGamePad(0);
	}
}

void GamepadInput::shutdown()
{
	description_window.shutdown();
}

// Axis deltas, trigger deltas, and button state transitions are tracked.
void GamepadInput::update_inputs()
{
	if (!gamepad)
		return;

	// Axis deltas are tracked only when movement occurs (zero values are ignored)
	if (gamepad->getAxesLeftDelta().length2() > 0)
		last_axes_left_delta = gamepad->getAxesLeftDelta();

	if (gamepad->getAxesRightDelta().length2() > 0)
		last_axes_right_delta = gamepad->getAxesRightDelta();

	// Trigger deltas are tracked only on press (positive delta)
	if (gamepad->getTriggerLeftDelta() > 0)
		last_trigger_left_delta = gamepad->getTriggerLeftDelta();

	if (gamepad->getTriggerRightDelta() > 0)
		last_trigger_right_delta = gamepad->getTriggerRightDelta();

	// All buttons are iterated to track state transitions:
	// - isButtonDown: first frame the button is pressed
	// - isButtonPressed: held down (fires every frame while held)
	// - isButtonUp: first frame the button is released
	for (int i = 0; i < Input::NUM_GAMEPAD_BUTTONS; i++)
	{
		Input::GAMEPAD_BUTTON i_button = Input::GAMEPAD_BUTTON(i);

		if (gamepad->isButtonDown(i_button))
			last_button_down = getGamePadButtonName(i_button);

		// Pressed buttons are tracked in a set to distinguish initial press from hold
		if (gamepad->isButtonPressed(i_button) && !pressed_buttons.contains(i))
		{
			pressed_buttons.append(i);
			last_button_pressed = getGamePadButtonName(i_button);
		}

		if (gamepad->isButtonUp(i_button))
		{
			pressed_buttons.remove(i);
			last_button_up = getGamePadButtonName(i_button);
		}
	}
}

// Touch input from gamepad touchpad is visualized (e.g., PS4/PS5 controllers).
void GamepadInput::draw_touches()
{
	if (gamepad->getNumTouches() == 0)
	{
		canvas_group->setHidden(true);
		return;
	}

	canvas_group->setHidden(false);
	canvas->clear();

	const static int number_of_colors = 10;
	const static Unigine::Math::vec4 colors[number_of_colors] = {
		Unigine::Math::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		Unigine::Math::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		Unigine::Math::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		Unigine::Math::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		Unigine::Math::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		Unigine::Math::vec4(1.0f, 0.0f, 1.0f, 1.0f),
		Unigine::Math::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		Unigine::Math::vec4(0.5f, 0.0f, 0.0f, 1.0f),
		Unigine::Math::vec4(0.0f, 0.5f, 0.0f, 1.0f),
		Unigine::Math::vec4(0.0f, 0.0f, 0.5f, 1.0f), };

	auto draw_circle = [](vec2 pos, WidgetCanvasPtr canvas, vec4 color, float pressure)
		{
			int polygon = canvas->addPolygon();
			canvas->setPolygonColor(polygon, color);
			const int num = 10;
			const float radius = 10;
			for (int i = 0; i < num; i++)
			{
				float s = Math::sin(Consts::PI2 * i / num) * radius * pressure + pos.x * canvas->getWidth();
				float c = Math::cos(Consts::PI2 * i / num) * radius * pressure + pos.y * canvas->getHeight();
				canvas->addPolygonPoint(polygon, vec3(s, c, 0.0f));
			}
		};

	for (int i = 0; i < gamepad->getNumTouches(); i++)
	{
		for (int j = 0; j < gamepad->getNumTouchFingers(i); j++)
		{
			if (gamepad->getTouchPressure(i, j) > Consts::EPS)
			{
				vec4 color = colors[i * gamepad->getNumTouches() + j];
				draw_circle(gamepad->getTouchPosition(i, j), canvas, color, gamepad->getTouchPressure(i, j));
			}
		}
	}
}

// Device type enum is converted to human-readable string.
String GamepadInput::getDeviceName(Input::DEVICE t)
{
	String ans = "";

	switch (t)
	{
	case Unigine::Input::DEVICE_UNKNOWN:
		ans = "UNKNOWN";
		break;
	case Unigine::Input::DEVICE_GAME_CONTROLLER:
		ans = "GAME CONTROLLER";
		break;
	case Unigine::Input::DEVICE_WHEEL:
		ans = "WHEEL";
		break;
	case Unigine::Input::DEVICE_ARCADE_STICK:
		ans = "ARCADE STICK";
		break;
	case Unigine::Input::DEVICE_FLIGHT_STICK:
		ans = "FLIGHT STICK";
		break;
	case Unigine::Input::DEVICE_DANCE_PAD:
		ans = "DANCE PAD";
		break;
	case Unigine::Input::DEVICE_GUITAR:
		ans = "GUITAR";
		break;
	case Unigine::Input::DEVICE_DRUM_KIT:
		ans = "DRUM KIT";
		break;
	case Unigine::Input::DEVICE_VR:
		ans = "VR";
		break;
	default:
		break;
	}

	return ans;
}

// Gamepad model type enum is converted to human-readable string.
String GamepadInput::getModelName(InputGamePad::MODEL_TYPE t)
{
	String ans = "";

	switch (t)
	{
	case Unigine::InputGamePad::MODEL_TYPE_UNKNOWN:
		ans = "UNKNOWN";
		break;
	case Unigine::InputGamePad::MODEL_TYPE_XBOX_360:
		ans = "XBOX 360";
		break;
	case Unigine::InputGamePad::MODEL_TYPE_XBOX_ONE:
		ans = "XBOX ONE";
		break;
	case Unigine::InputGamePad::MODEL_TYPE_PS3:
		ans = "PS3";
		break;
	case Unigine::InputGamePad::MODEL_TYPE_PS4:
		ans = "PS4";
		break;
	case Unigine::InputGamePad::MODEL_TYPE_PS5:
		ans = "PS5";
		break;
	default:
		break;
	}

	return ans;
}

// Gamepad button enum is converted to human-readable string.
String GamepadInput::getGamePadButtonName(Input::GAMEPAD_BUTTON btn)
{
	String ans = "";

	switch (btn)
	{
	case Unigine::Input::GAMEPAD_BUTTON_A: ans = "A"; break;
	case Unigine::Input::GAMEPAD_BUTTON_B: ans = "B"; break;
	case Unigine::Input::GAMEPAD_BUTTON_X: ans = "X"; break;
	case Unigine::Input::GAMEPAD_BUTTON_Y: ans = "Y"; break;
	case Unigine::Input::GAMEPAD_BUTTON_BACK: ans = "BACK"; break;
	case Unigine::Input::GAMEPAD_BUTTON_START: ans = "START"; break;
	case Unigine::Input::GAMEPAD_BUTTON_DPAD_UP: ans = "DPAD_UP"; break;
	case Unigine::Input::GAMEPAD_BUTTON_DPAD_DOWN: ans = "DPAD_DOWN"; break;
	case Unigine::Input::GAMEPAD_BUTTON_DPAD_LEFT: ans = "DPAD_LEFT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_DPAD_RIGHT: ans = "DPAD_RIGHT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_THUMB_LEFT: ans = "THUMB_LEFT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_THUMB_RIGHT: ans = "THUMB_RIGHT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_SHOULDER_LEFT: ans = "SHOULDER_LEFT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_SHOULDER_RIGHT: ans = "SHOULDER_RIGHT"; break;
	case Unigine::Input::GAMEPAD_BUTTON_GUIDE: ans = "GUIDE"; break;
	case Unigine::Input::GAMEPAD_BUTTON_MISC1: ans = "MISC1"; break;
	case Unigine::Input::GAMEPAD_BUTTON_TOUCHPAD: ans = "TOUCHPAD"; break;
	default:
		break;
	}

	return ans;
}
