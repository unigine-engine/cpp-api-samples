#include "CraneControl.h"

#include "SlingRope.h"

#include <UnigineGame.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(CraneControl);

void CraneControl::init()
{
	if (rope.nullCheck())
	{
		Log::error("%s(): Rope node is empty!\n", __FUNCTION__);
		return;
	}
	rope_component = ComponentSystem::get()->getComponent<SlingRope>(rope);
	if (!rope_component)
	{
		Log::error("%s(): Rope node must have SlingRope component!\n", __FUNCTION__);
		return;
	}
}

void CraneControl::update()
{
	// collect turn input
	float turn_rate = turn_acceleration * Game::getIFps();
	int turn_direction = 0;

	if (Input::isKeyPressed(Input::KEY_J))
		turn_direction += 1;
	if (Input::isKeyPressed(Input::KEY_L))
		turn_direction += -1;

	// if turn_direction == 0, turn_speed tends to zero
	turn_speed = lerp(turn_speed, turn_direction * turn_max_speed, turn_rate);

	// rotate dynamic crane part
	auto rot = Mat4(rotateZ(turn_speed * Game::getIFps()));
	node->setTransform(rot * node->getTransform());

	if (rope_component)
	{
		// rope length control
		int lenght_change = 0;
		if (Input::isKeyPressed(Input::KEY_I))
			lenght_change -= 1;
		if (Input::isKeyPressed(Input::KEY_K))
			lenght_change += 1;

		if (lenght_change != 0)
		{
			float delta_length = lenght_change * rope_speed * Game::getIFps();
			rope_component->setLength(rope_component->getLength() + delta_length);
		}

		// bag control
		if (Input::isKeyDown(Input::KEY_O))
			rope_component->attachBag();
		if (Input::isKeyDown(Input::KEY_U))
			rope_component->detachBag();
	}
}
