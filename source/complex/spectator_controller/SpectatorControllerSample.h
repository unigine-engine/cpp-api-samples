#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include "SpectatorController.h"

class SpectatorControllerSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(SpectatorControllerSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();

	SampleDescriptionWindow description_window;
	Unigine::Input::MOUSE_HANDLE mouse_handle_at_init;

	SpectatorController* spectator {nullptr};

	bool is_controlled = false;
	bool is_collided = false;

	float min_mouse_sensetivity = 0.1f;
	float max_mouse_sensetivity = 1.0f;
	float current_mouse_sensetivity = 0.0f;

	float min_turning = 15.0f;
	float max_turning = 120.0f;
	float current_turning = 0.0f;

	float min_velocity = 1.0f;
	float max_velocity = 4.0f;
	float current_velocity = 0.0f;

	float min_sprint_velocity = 5.0f;
	float max_sprint_velocity = 10.0f;
	float current_sprint_velocity = 0.0f;
};