// Sample UI for configuring spectator camera parameters at runtime.
// Provides sliders for mouse sensitivity, turning speed, velocity, and sprint velocity,
// plus checkboxes for input control and collision detection toggles.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include "SpectatorController.h"

// Creates parameter adjustment GUI for SpectatorController: sliders for movement
// settings and checkboxes for input/collision toggles.
class SpectatorControllerSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(SpectatorControllerSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	SampleDescriptionWindow description_window;			// GUI window for parameter controls
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;	// Original mouse mode for restoration

	SpectatorController* spectator {nullptr};	// The controlled spectator component

	bool is_controlled = false;		// Whether input control is enabled
	bool is_collided = false;		// Whether collision detection is enabled

	// Mouse sensitivity slider range and current value
	float min_mouse_sensetivity = 0.1f;		// Note: typo in original - "sensetivity"
	float max_mouse_sensetivity = 1.0f;
	float current_mouse_sensetivity = 0.0f;

	// Turning speed slider range and current value
	float min_turning = 15.0f;
	float max_turning = 120.0f;
	float current_turning = 0.0f;

	// Base velocity slider range and current value
	float min_velocity = 1.0f;
	float max_velocity = 4.0f;
	float current_velocity = 0.0f;

	// Sprint velocity slider range and current value
	float min_sprint_velocity = 5.0f;
	float max_sprint_velocity = 10.0f;
	float current_sprint_velocity = 0.0f;
};