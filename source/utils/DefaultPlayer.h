#pragma once

#include <UnigineComponentSystem.h>

class DefaultPlayer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DefaultPlayer, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Switch, mouse_button, 3, "UNKNOWN,LEFT,MIDDLE,RIGHT,DCLICK,AUX_0,AUX_1,AUX_2,AUX_3");
	PROP_PARAM(Node, player);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::PlayerPtr player_camera = nullptr;

	Unigine::Input::MOUSE_BUTTON spectator_mode_button = Unigine::Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT;
	bool prev_state = false;
	
	bool init_player_controlled = false;
	bool init_mouse_enabled = false;
	Unigine::Input::MOUSE_HANDLE init_mouse_handle;
};
