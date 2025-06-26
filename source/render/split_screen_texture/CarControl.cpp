#include "CarControl.h"
#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(CarControl);

using namespace Unigine;
using namespace Math;

void CarControl::init()
{
	init_input_keys();
}


void CarControl::init_input_keys()
{
	if (!use_arrows.get())
	{
		key_forward = Input::KEY_W;
		key_back = Input::KEY_S;
		key_left = Input::KEY_A;
		key_right = Input::KEY_D;

	} else
	{
		key_forward = Input::KEY_UP;
		key_back = Input::KEY_DOWN;
		key_left = Input::KEY_LEFT;
		key_right = Input::KEY_RIGHT;
	}
}

void CarControl::update()
{
	if (Console::isActive())
		return;
	int fb = get_movement_command();

	int rl = get_rotate_command();

	float ifps = Game::getIFps();
	forward = lerp(forward, Scalar(max_speed.get() * fb), Scalar(ifps * acceleration_factor.get()));
	rotation = lerp(rotation, float(max_rotation.get() * rl), ifps);

	node->rotate(0, 0, rotation * ifps);
	node->translate(0, forward * ifps, 0);
}

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

void CarControl::shutdown()
{
}