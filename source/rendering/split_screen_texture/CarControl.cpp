// Simple car movement controller supporting two key schemes (WASD or arrows).
// Uses lerp for smooth acceleration/deceleration and rotation.

#include "CarControl.h"
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(CarControl);

using namespace Unigine;
using namespace Math;

// Input key bindings are configured based on the use_arrows setting.
void CarControl::init()
{
	init_input_keys();
}


// Key assignments are selected based on player preference (WASD or arrow keys).
void CarControl::init_input_keys()
{
	// WASD keys for player 1 (default)
	if (!use_arrows.get())
	{
		key_forward = Input::KEY_W;
		key_back = Input::KEY_S;
		key_left = Input::KEY_A;
		key_right = Input::KEY_D;

	} else
	{
		// Arrow keys for player 2
		key_forward = Input::KEY_UP;
		key_back = Input::KEY_DOWN;
		key_left = Input::KEY_LEFT;
		key_right = Input::KEY_RIGHT;
	}
}

// Input is polled; velocity and rotation are smoothly interpolated toward target values.
void CarControl::update()
{
	// Skip input processing when console is open to avoid interfering with typing
	if (Console::isActive())
		return;

	// Get current input directions (-1, 0, or 1)
	int fb = get_movement_command();
	int rl = get_rotate_command();

	// Lerp provides smooth acceleration/deceleration for natural-feeling movement
	float ifps = Game::getIFps();
	forward = lerp(forward, Scalar(max_speed.get() * fb), Scalar(ifps * acceleration_factor.get()));
	rotation = lerp(rotation, float(max_rotation.get() * rl), ifps);

	// Apply rotation around Z axis (yaw) and translate along local Y (forward)
	node->rotate(0, 0, rotation * ifps);
	node->translate(0, forward * ifps, 0);
}

// Forward/back key state is converted to direction value.
int CarControl::get_movement_command() const
{
	int fb = 0;
	if (Input::isKeyPressed(key_forward))
	{
		fb = 1;
	} else if (Input::isKeyPressed(key_back))
	{
		fb = -1;
	}
	return fb;
}

// Left/right key state is converted to rotation direction value.
int CarControl::get_rotate_command() const
{
	int rl = 0;
	if (Input::isKeyPressed(key_left))
	{
		rl = 1;
	} else if (Input::isKeyPressed(key_right))
	{
		rl = -1;
	}
	return rl;
}

// No cleanup needed; component state is automatically reset.
void CarControl::shutdown()
{
}