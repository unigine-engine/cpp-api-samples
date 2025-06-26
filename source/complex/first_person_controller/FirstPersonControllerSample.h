#pragma once

#include <UnigineComponentSystem.h>


class FirstPersonControllerSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(FirstPersonControllerSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);


private:
	void init();
	void shutdown();

private:
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;
	bool current_mouse_grab_state{false};
	bool mouse_grab_state_at_init{false};
};
