#include "BoatControl.h"
#include <UnigineConsole.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(BoatControl);

using namespace Unigine;
using namespace Math;


void BoatControl::init()
{

}

void BoatControl::update()
{
	if (Console::isActive())
		return;


	int fb = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	int rl = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));

	float ifps = Game::getIFps();
	forward = lerp(forward, Scalar(max_speed.get() * fb), Scalar(ifps * acceleration_factor.get()));
	if (Math::abs(forward) < min_speed_for_rot.get())
		rl = 0;
	if (invert_rear_dir.get() && forward < 0.0)
		rl = -rl;

	rotation = lerp(rotation, float(max_rotation.get() * rl), ifps);

	node->rotate(0, 0, rotation * ifps);
	node->translate(0, forward * ifps, 0);
}

void BoatControl::shutdown()
{

}