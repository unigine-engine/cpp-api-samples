// Manages mouse input state for the first-person controller sample.
// Saves and restores mouse grab/handle modes between sessions.

#pragma once

#include <UnigineComponentSystem.h>

// Handles mouse capture setup and teardown for first-person controls.
class FirstPersonControllerSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(FirstPersonControllerSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

private:
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;	// Original mouse handle mode
	bool current_mouse_grab_state{false};				// Current grab state (unused)
	bool mouse_grab_state_at_init{false};				// Original grab state for restoration
};
