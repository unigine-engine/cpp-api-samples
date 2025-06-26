#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>


class PlayerPersecutorSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(PlayerPersecutorSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, rotator, "");
	PROP_PARAM(Node, persecutor, "", "", "", "filter=PlayerDummy");

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow description_window;
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;
	bool current_mouse_grab_state{ false };
	bool mouse_grab_state_at_init{ false };
};
