#include "GamepadInput.h"
#include "Car.h"
#include "../../utils/SimpleInformationBox.h"
#include <UnigineGame.h>

REGISTER_COMPONENT(GamepadInput);

using namespace Unigine;
using namespace Math;

void GamepadInput::setFilter(float f)
{
	if (gamepad)
	{
		gamepad->setFilter(f);
	}
}

void GamepadInput::setLowFrequency(float f)
{
	low_frequency = clamp(f, 0.0f, 1.0f);
}

void GamepadInput::setHighFrequency(float f)
{
	high_frequency = clamp(f, 0.0f, 1.0f);
}

void GamepadInput::setDuration(float d)
{
	vibration_duration = d;
	if (vibration_duration < 0.0f)
		vibration_duration = 0.0f;
}


void GamepadInput::init()
{
	Physics::setFrozenLinearVelocity(0.1f);
	Physics::setFrozenAngularVelocity(0.1f);

	info = getComponent<SimpleInformationBox>(node);

	info->setWindowTitle("Gamepad Input Sample");
	info->setColumnsCount(1);
	info->setWidth(300);
	info->pushBackAboutInfo("This sample demostrates the simple usage of Gamepad input.");

	auto group = info->getParametersGroupBox(0);
	canvas = WidgetCanvas::create();
	group->addChild(canvas);
	canvas->setWidth(group->getWidth());
	canvas->setHeight(toInt(group->getWidth() * 0.5));
	canvas->setHidden(true);

	auto slider = info->addSlider(0, "Filter ", 0.01f, "Buttons sensitivity threshold");
	slider->getEventChanged().connect(widget_connections, [this, slider]() {
		setFilter(slider->getValue() * 0.01f);
		});
	slider = info->addSlider(0, "Low Frequency ", 0.01f, "Power of vibration for low frequency");
	slider->getEventChanged().connect(widget_connections, [this, slider]() {
		setLowFrequency(slider->getValue() * 0.01f);
		});
	slider = info->addSlider(0, "High Frequency ", 0.01f, "Power of vibration for high frequency");
	slider->getEventChanged().connect(widget_connections, [this, slider]() {
		setHighFrequency(slider->getValue() * 0.01f);
		});
	slider = info->addSlider(0, "Duration ", 1.0f, "Vibration duration in ms");
	slider->getEventChanged().connect(widget_connections, [this, slider]() {
		setDuration((float)slider->getValue());
		});
	slider->setMinValue(1);
	slider->setMaxValue(1000);
	slider->setValue(1);

	car = getComponent<Car>(World::getNodeByName("car"));

	if (Input::getNumGamePads() > 0)
	{
		gamepad = Input::getGamePad(0);
	}
	else
	{
		Log::error("GamepadInput::init(): No available gamepads!\n");
		return;
	}

}

void GamepadInput::update()
{
	update_inputs();

	info->clearParametersInfo(0);

	if (gamepad && gamepad->isAvailable())
	{
		draw_touches();

		info->pushBackParametersInfo(0, gamepad->getName(), SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Press Left and Right triggers to run motors", SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Press <b>X button</b> to respawn", SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Press <b>Y button</b> to vibrate", SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Number", String::itoa(gamepad->getNumber()));
		info->pushBackParametersInfo(0, "Player Index", String::itoa(gamepad->getPlayerIndex()));
		info->pushBackParametersInfo(0, "Device Type", getDeviceName(gamepad->getDeviceType()));
		info->pushBackParametersInfo(0, "Model Type", getModelName(gamepad->getModelType()));
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Axes", SimpleInformationBox::INFO_ALIGN::CENTER);

		info->pushBackParametersInfo(0, "Left X", String::ftoa(gamepad->getAxesLeft().x));

		info->pushBackParametersInfo(0, "Left Y", String::ftoa(gamepad->getAxesLeft().y));

		info->pushBackParametersInfo(0, "Right X", String::ftoa(gamepad->getAxesRight().x));

		info->pushBackParametersInfo(0, "Right Y", String::ftoa(gamepad->getAxesRight().y));

		info->pushBackParametersInfo(0, "Left X Last Delta", String::ftoa(last_axes_left_delta.x));

		info->pushBackParametersInfo(0, "Left Y Last Delta", String::ftoa(last_axes_left_delta.y));

		info->pushBackParametersInfo(0, "Right X Last Delta", String::ftoa(last_axes_right_delta.x));

		info->pushBackParametersInfo(0, "Right Y Last Delta", String::ftoa(last_axes_right_delta.y));

		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Triggers", SimpleInformationBox::INFO_ALIGN::CENTER);

		info->pushBackParametersInfo(0, "Left", String::ftoa(gamepad->getTriggerLeft()));

		info->pushBackParametersInfo(0, "Right", String::ftoa(gamepad->getTriggerRight()));

		info->pushBackParametersInfo(0, "Left Last Delta", String::ftoa(last_trigger_left_delta));

		info->pushBackParametersInfo(0, "Right Last Delta", String::ftoa(last_trigger_right_delta));
		info->pushBackWhiteSpaceLineParametersInfo(0);

		info->pushBackParametersInfo(0, "Buttons", SimpleInformationBox::INFO_ALIGN::CENTER);

		info->pushBackParametersInfo(0, "Last Button Down", last_button_down, SimpleInformationBox::INFO_ALIGN::LEFT);
		info->pushBackParametersInfo(0, "Last Button Pressed", last_button_pressed, SimpleInformationBox::INFO_ALIGN::LEFT);
		info->pushBackParametersInfo(0, "Last Button Up", last_button_up, SimpleInformationBox::INFO_ALIGN::LEFT);
		info->pushBackParametersInfo(0, "                   ", SimpleInformationBox::INFO_ALIGN::CENTER);
		info->pushBackWhiteSpaceLineParametersInfo(0);
		info->showAdditionalWidgets(0, true);

		if (gamepad->isButtonPressed(Input::GAMEPAD_BUTTON_X))
			car->respawn();

		if(gamepad->isButtonDown(Input::GAMEPAD_BUTTON_Y))
			gamepad->setVibration(low_frequency, high_frequency, vibration_duration);

		car->setAngle(gamepad->getAxesLeft().x);
		car->setVelocity(gamepad->getTriggerRight());
		car->setBrake(gamepad->getTriggerLeft());
	}
	else
	{
		info->pushBackParametersInfo(0, "Connect gamepad.", SimpleInformationBox::INFO_ALIGN::CENTER);
		info->showAdditionalWidgets(0, false);

		if(!gamepad && Input::getNumGamePads())
			gamepad = Input::getGamePad(0);
	}
}

void GamepadInput::update_inputs()
{
	if (!gamepad)
		return;

	if (gamepad->getAxesLeftDelta().length2() > 0)
		last_axes_left_delta = gamepad->getAxesLeftDelta();

	if (gamepad->getAxesRightDelta().length2() > 0)
		last_axes_right_delta = gamepad->getAxesRightDelta();

	if (gamepad->getTriggerLeftDelta() > 0)
		last_trigger_left_delta = gamepad->getTriggerLeftDelta();

	if (gamepad->getTriggerRightDelta() > 0)
		last_trigger_right_delta = gamepad->getTriggerRightDelta();

	for (int i = 0; i < Input::NUM_GAMEPAD_BUTTONS; i++)
	{
		Input::GAMEPAD_BUTTON i_button = Input::GAMEPAD_BUTTON(i);

		if (gamepad->isButtonDown(i_button))
			last_button_down = getGamePadButtonName(i_button);

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

void GamepadInput::draw_touches()
{
	if (gamepad->getNumTouches() == 0)
	{
		canvas->setHidden(true);
		return;
	}

	canvas->setHidden(false);
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

	auto draw_circle = [](vec2 pos, WidgetCanvasPtr canvas, vec4 color,float pressure)
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
			if (gamepad->getTouchPressure(i,j) > Consts::EPS)
			{
				vec4 color = colors[i * gamepad->getNumTouches() + j];
				draw_circle(gamepad->getTouchPosition(i,j), canvas, color, gamepad->getTouchPressure(i,j));
			}
		}
	}
}

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
