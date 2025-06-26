#pragma once

#include <UnigineComponentSystem.h>

class SimpleInformationBox;

class KeyboardAndMouseInput : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(KeyboardAndMouseInput, Unigine::ComponentBase);

	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void check_last_mouse_button();
	void check_last_key();

	void on_text_pressed(unsigned int unicode);

	Unigine::String last_mouse_button_down = "";
	Unigine::String last_mouse_button_pressed = "";
	Unigine::String last_mouse_button_up = "";
	Unigine::String last_input_symbol = "";
	Unigine::String last_key_down = "";
	Unigine::String last_key_pressed = "";
	Unigine::String last_key_up = "";

	Unigine::HashSet<unsigned int> pressed_keys;
	Unigine::HashSet<int> pressed_mouse_buttons;
	Unigine::Math::ivec2 mouse_position;
	Unigine::Math::ivec2 last_mouse_delta_coordinates = { 0, 0 };
	Unigine::Math::vec2 last_mouse_delta = { 0, 0 };
	int last_mouse_wheel = 0;
	int last_mouse_wheel_horizontal = 0;

	SimpleInformationBox *info = nullptr;
	Unigine::EventConnections widget_connections;
};
