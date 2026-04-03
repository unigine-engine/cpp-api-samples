// Simple car controller for split-screen racing demonstration.
// Handles keyboard input (WASD or arrows) to move and rotate
// the car with configurable speed and acceleration.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineInput.h>

class CarControl: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CarControl, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Movement parameters
	PROP_PARAM(Float, max_speed, 10.f);
	PROP_PARAM(Float, max_rotation, 50.f);
	PROP_PARAM(Float, acceleration_factor, 0.5f);
	// When true, uses arrow keys instead of WASD
	PROP_PARAM(Toggle, use_arrows, false);

private:
	void init();
	void update();
	void shutdown();

	// Returns -1, 0, or 1 based on forward/back input
	int get_movement_command() const;
	// Returns -1, 0, or 1 based on left/right input
	int get_rotate_command() const;
	// Binds keys based on use_arrows setting
	void init_input_keys();

	// Current movement state
	Unigine::Math::Scalar forward = 0.f;
	float rotation = 0.f;
	// Configured input keys
	Unigine::Input::KEY key_forward;
	Unigine::Input::KEY key_back;
	Unigine::Input::KEY key_left;
	Unigine::Input::KEY key_right;
};
