#include "KeyboardAndMouseInput.h"
#include "../../utils/SimpleInformationBox.h"
#include <UnigineWidgets.h>

REGISTER_COMPONENT(KeyboardAndMouseInput);

using namespace Unigine;
using namespace Math;

void KeyboardAndMouseInput::init()
{
	info = getComponent<SimpleInformationBox>(node);

	info->setWindowTitle("Keyboar and Mouse Input Sample");
	info->setColumnsCount(1);
	info->setWidth(300);
	info->pushBackAboutInfo("This sample demostrates the simple usage of Keyboard and Mouse input.");
	auto combobox = info->addCombobox(0, "Change Mouse Handle ");
	combobox->getEventChanged().connect(widget_connections, [combobox]() {
		Input::setMouseHandle((Input::MOUSE_HANDLE)combobox->getCurrentItem());
		});
	combobox->addItem("GRAB");
	combobox->addItem("SOFT");
	combobox->addItem("USER");

	Input::getEventTextPress().connect(widget_connections, this, &KeyboardAndMouseInput::on_text_pressed);
}

void KeyboardAndMouseInput::update()
{
	info->clearParametersInfo(0);

	// reset keys info
	check_last_mouse_button();
	info->pushBackParametersInfo(0, "Last Mouse Button Down", last_mouse_button_down);
	info->pushBackParametersInfo(0, "Last Mouse Button Pressed", last_mouse_button_pressed);
	info->pushBackParametersInfo(0, "Last Mouse Button Up", last_mouse_button_up);
	info->pushBackWhiteSpaceLineParametersInfo(0);

	check_last_key();
	info->pushBackParametersInfo(0, "Last Input Symbol", last_input_symbol);
	info->pushBackParametersInfo(0, "Last Down Key Code", last_key_down);
	info->pushBackParametersInfo(0, "Last Pressed Key Code", last_key_pressed);
	info->pushBackParametersInfo(0, "Last Up Key Code", last_key_up);
	info->pushBackWhiteSpaceLineParametersInfo(0);

	// reset window position coordinates
	info->pushBackParametersInfo(0, "Window Position X", String::itoa(WindowManager::getMainWindow()->getPosition().x));
	info->pushBackParametersInfo(0, "Window Position Y", String::itoa(WindowManager::getMainWindow()->getPosition().y));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	// reset mouse position coordinates in screen space
	info->pushBackParametersInfo(0, "Screen Space Mouse Position X", String::itoa(Input::getMousePosition().x));
	info->pushBackParametersInfo(0, "Screen Space Mouse Position Y", String::itoa(Input::getMousePosition().y));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	// reset mouse position coordinates in window space
	info->pushBackParametersInfo(0, "Window Space Mouse Position X", String::itoa(Gui::getCurrent()->getMouseX()));
	info->pushBackParametersInfo(0, "Window Space Mouse Position Y", String::itoa(Gui::getCurrent()->getMouseY()));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	// reset mouse deltas
	if (Input::getMouseDeltaPosition().length2() > 0)
		last_mouse_delta_coordinates = Input::getMouseDeltaPosition();

	info->pushBackParametersInfo(0, "Last Mouse Coordinates Delta X", String::itoa(last_mouse_delta_coordinates.x));
	info->pushBackParametersInfo(0, "Last Mouse Coordinates Delta Y", String::itoa(last_mouse_delta_coordinates.y));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	if (Input::getMouseDeltaRaw().length2() > 0)
		last_mouse_delta = vec2(Input::getMouseDeltaRaw());

	info->pushBackParametersInfo(0, "Last Mouse Delta X", String::ftoa(last_mouse_delta.x));
	info->pushBackParametersInfo(0, "Last Mouse Delta Y", String::ftoa(last_mouse_delta.y));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	// reset mouse axes direction
	if (Input::getMouseWheel() != 0)
		last_mouse_wheel = Input::getMouseWheel();

	if (Input::getMouseWheelHorizontal() != 0)
		last_mouse_wheel_horizontal = Input::getMouseWheelHorizontal();

	info->pushBackParametersInfo(0, "Last Mouse Wheel", String::itoa(last_mouse_wheel));
	info->pushBackParametersInfo(0, "Last Mouse Wheel Horizontal", String::itoa(last_mouse_wheel_horizontal));
	info->pushBackWhiteSpaceLineParametersInfo(0);

	if (Input::getMouseHandle() == Input::MOUSE_HANDLE_GRAB)
	{
		if(Input::isMouseGrab())
			info->pushBackParametersInfo(0, "Mouse Handle: GRAB (press ESC to show cursor)");
		else
			info->pushBackParametersInfo(0, "Mouse Handle: GRAB");
	}
	if (Input::getMouseHandle() == Input::MOUSE_HANDLE_SOFT)
		info->pushBackParametersInfo(0, "Mouse Handle: SOFT");
	if (Input::getMouseHandle() == Input::MOUSE_HANDLE_USER)
		info->pushBackParametersInfo(0, "Mouse Handle: USER");
}

void KeyboardAndMouseInput::shutdown()
{
	widget_connections.disconnectAll();
}

void KeyboardAndMouseInput::check_last_mouse_button()
{
	for (int i = 0; i < Input::MOUSE_NUM_BUTTONS; i++)
	{
		if (Input::isMouseButtonDown((Input::MOUSE_BUTTON)i))
			last_mouse_button_down = Input::getMouseButtonName((Input::MOUSE_BUTTON)i);

		if (Input::isMouseButtonPressed((Input::MOUSE_BUTTON)i) && !pressed_mouse_buttons.contains(i))
		{
			pressed_mouse_buttons.append(i);
			last_mouse_button_pressed = Input::getMouseButtonName((Input::MOUSE_BUTTON)i);
		}

		if (Input::isMouseButtonUp((Input::MOUSE_BUTTON)i))
		{
			pressed_mouse_buttons.remove(i);
			last_mouse_button_up = Input::getMouseButtonName((Input::MOUSE_BUTTON)i);
		}
	}
}

void KeyboardAndMouseInput::check_last_key()
{
	for (unsigned int i = 0; i < Input::KEY_ANY_SHIFT; i++)
	{
		if (Input::isKeyDown((Input::KEY)i))
			last_key_down = Input::getKeyName((Input::KEY)i);

		if (Input::isKeyPressed((Input::KEY)i) && !pressed_keys.contains(i))
		{
			pressed_keys.append(i);
			last_key_pressed = Input::getKeyName((Input::KEY)i);
		}

		if (Input::isKeyUp((Input::KEY)i))
		{
			pressed_keys.remove(i);
			last_key_up = Input::getKeyName((Input::KEY)i);
		}
	}
}

void KeyboardAndMouseInput::on_text_pressed(unsigned int unicode)
{
	last_input_symbol = String::unicodeToUtf8(unicode);
}
