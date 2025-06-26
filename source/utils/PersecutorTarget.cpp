#include "PersecutorTarget.h"
#include <UnigineNode.h>
#include <UnigineGame.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(PersecutorTarget);

void PersecutorTarget::init()
{
	player = checked_ptr_cast<PlayerPersecutor>(node);
	if (!player)
	{
		Log::error("%s: component for %s(%d): can be only on PlayerPersecutor node!\n", __FUNCTION__, node->getName(), node->getID());
		return;
	}
	if (!target.get())
	{
		Log::error("%s: component for %s(%d): target is empty!\n", __FUNCTION__, node->getName(), node->getID());
		return;
	}
	target_node = NodeDummy::create();
	player->setTarget(target_node);
	valid = true;

	distance_spread = (player->getMaxDistance() - player->getMinDistance()) * 0.5f;
}

void PersecutorTarget::update()
{
	if (!valid || !target.get())
		return;

	if (Console::isActive() || Game::getPlayer() != player)
		return;

	if (Input::isKeyPressed(Input::KEY_P))
		fixed = !fixed;

	float ifps = Game::getIFps();

	if (lerp_pos)
		target_node->setWorldPosition(lerp(target_node->getWorldPosition(), target->getWorldPosition(), float(lerp_pos_k * ifps)));
	else
		target_node->setWorldPosition(target->getWorldPosition());

	if (wait_angle)
	{
		ivec2 delta = Input::getMouseDeltaPosition();
		ivec4 mdx = ivec4(delta.x, delta.y, Input::getMouseWheel(), Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT));

		if (mdx != ivec4_zero || fixed)
		{
			if (!player->isFixed())
				player->setFixed(true);
			timer = wait_angle_time;
		}
		else
		{
			timer -= ifps;
			if (timer <= 0 && player->isFixed())
			{
				player->setFixed(false);
			}
		}
	}

	if (wheel_distance)
	{
		int dt = Input::getMouseWheel();
		if (dt)
		{
			float new_distance = player->getDistance() - dt * ifps * wheel_distance_speed;
			new_distance = clamp(new_distance, wheel_distance_clamp.get().x, wheel_distance_clamp.get().y);
			player->setDistance(new_distance);
			player->setMinDistance(new_distance - distance_spread);
			player->setMaxDistance(new_distance + distance_spread);
		}
	}
}

void PersecutorTarget::shutdown()
{
	if (target_node)
		target_node.deleteLater();
}

