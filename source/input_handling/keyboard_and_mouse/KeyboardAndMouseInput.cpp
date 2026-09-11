// Displays keyboard and mouse input state in real-time. Tracks key/button transitions
// (down, pressed, up), mouse position in screen and window space, wheel scrolling,
// and raw mouse deltas. Supports text input events for unicode character capture.

#include "KeyboardAndMouseInput.h"
#include <UnigineWindowManager.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(KeyboardAndMouseInput);

using namespace Unigine;
using namespace Math;

void KeyboardAndMouseInput::init()
{
	description_window.createWindow();

	// Mouse handle modes:
	// GRAB - cursor is hidden and locked to window center
	// SOFT - cursor is visible, can move freely
	// USER - application manages cursor visibility manually
	description_window.addSwitchParameter("Mouse Handle", "Change how the cursor is captured by the window.",
		1, {"GRAB", "SOFT", "USER"},
		[](int idx) {
			Input::setMouseHandle((Input::MOUSE_HANDLE)idx);
		});

	auto group = WidgetGroupBox::create("Input State", 8, 8);
	description_window.getWindow()->addChild(group, Gui::ALIGN_LEFT);
	info_label = WidgetLabel::create();
	info_label->setFontRich(1);
	info_label->setFontWrap(1);
	group->addChild(info_label, Gui::ALIGN_EXPAND);

	Input::getEventTextPress().connect(this, &KeyboardAndMouseInput::on_text_pressed);
}

void KeyboardAndMouseInput::update()
{
	check_last_mouse_button();
	check_last_key();

	if (Input::getMouseDeltaPosition().length2() > 0)
		last_mouse_delta_coordinates = Input::getMouseDeltaPosition();

	if (Input::getMouseDeltaRaw().length2() > 0)
		last_mouse_delta = vec2(Input::getMouseDeltaRaw());

	if (Input::getMouseWheel() != 0)
		last_mouse_wheel = Input::getMouseWheel();

	if (Input::getMouseWheelHorizontal() != 0)
		last_mouse_wheel_horizontal = Input::getMouseWheelHorizontal();

	String handle_text;
	if (Input::getMouseHandle() == Input::MOUSE_HANDLE_GRAB)
		handle_text = Input::isMouseGrab() ? "GRAB (press ESC to show cursor)" : "GRAB";
	else if (Input::getMouseHandle() == Input::MOUSE_HANDLE_SOFT)
		handle_text = "SOFT";
	else
		handle_text = "USER";

	ivec2 win_pos = WindowManager::getMainWindow()->getPosition();
	ivec2 mouse_pos = Input::getMousePosition();

	String text = String::format(
		"Last Mouse Button Down: %s\n"
		"Last Mouse Button Pressed: %s\n"
		"Last Mouse Button Up: %s\n"
		"\n"
		"Last Input Symbol: %s\n"
		"Last Down Key Code: %s\n"
		"Last Pressed Key Code: %s\n"
		"Last Up Key Code: %s\n"
		"\n"
		"Window Position: %d, %d\n"
		"Screen Space Mouse: %d, %d\n"
		"Window Space Mouse: %d, %d\n"
		"\n"
		"Last Mouse Coordinates Delta: %d, %d\n"
		"Last Mouse Delta: %s, %s\n"
		"\n"
		"Last Mouse Wheel: %d\n"
		"Last Mouse Wheel Horizontal: %d\n"
		"\n"
		"Mouse Handle: %s",
		last_mouse_button_down.get(),
		last_mouse_button_pressed.get(),
		last_mouse_button_up.get(),
		last_input_symbol.get(),
		last_key_down.get(),
		last_key_pressed.get(),
		last_key_up.get(),
		win_pos.x, win_pos.y,
		mouse_pos.x, mouse_pos.y,
		Gui::getCurrent()->getMouseX(), Gui::getCurrent()->getMouseY(),
		last_mouse_delta_coordinates.x, last_mouse_delta_coordinates.y,
		String::ftoa(last_mouse_delta.x).get(), String::ftoa(last_mouse_delta.y).get(),
		last_mouse_wheel,
		last_mouse_wheel_horizontal,
		handle_text.get());

	info_label->setText(text.get());
}

void KeyboardAndMouseInput::shutdown()
{
	description_window.shutdown();
}

// Track mouse button state transitions across frames.
// Down/Up fire once on state change, Pressed fires continuously while held.
void KeyboardAndMouseInput::check_last_mouse_button()
{
	for (int i = 0; i < Input::MOUSE_NUM_BUTTONS; i++)
	{
		if (Input::isMouseButtonDown((Input::MOUSE_BUTTON)i))
			last_mouse_button_down = Input::getMouseButtonName((Input::MOUSE_BUTTON)i);

		// Use HashSet to distinguish first press from continued hold
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

// Called for each text character input (handles unicode properly).
// This is separate from key events - captures actual typed characters
// including those requiring modifiers (shift, alt) or IME input.
void KeyboardAndMouseInput::on_text_pressed(unsigned int unicode)
{
	last_input_symbol = String::unicodeToUtf8(unicode);
}
