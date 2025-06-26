#include "InteractorComponent.h"

#include "UnigineVisualizer.h"

REGISTER_COMPONENT(InteractorComponent);

using namespace Unigine;
using namespace Math;

const float interaction_distance = 10;

void InteractorComponent::init()
{
	mouse_handle = Input::getMouseHandle();
	Visualizer::setEnabled(true);

	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	player = checked_ptr_cast<Player>(node);
	if (!player)
	{
		Log::error("InteractorComponent::init: this component should be placed only on Player "
				   "node. '%s'%d\n",
			node->getName(), node->getID());
		return;
	}
}

void InteractorComponent::update()
{

	if (Input::isKeyDown(Input::KEY_F))
	{
		ivec2 mouse = Input::getMousePosition();
		Vec3 p0 = player->getWorldPosition();
		Vec3 p1 = p0
			+ Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y)) * interaction_distance;

		obj = World::getIntersection(p0, p1, interactable_intersection_mask.get(), intersection);
	}
	if (Input::isKeyPressed(Input::KEY_F))
	{
		if (obj)
		{
			ivec2 mouse = Input::getMousePosition();
			Vec3 p0 = player->getWorldPosition();
			Vec3 p1 = p0
				+ Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y)) * interaction_distance;

			World::getIntersection(p0, p1, ~0, intersection);
			comp_interactable = ComponentSystem::get()->getComponent<Interactable>(obj);
			if (comp_interactable)
			{
				comp_interactable->use(player, (vec3)intersection->getPoint());
			}
		}
	}
	if (Input::isKeyUp(Input::KEY_F))
	{
		obj = nullptr;
	}
}

void InteractorComponent::post_update()
{
	Visualizer::renderVector(player->getWorldPosition(),
		player->getWorldPosition() + (Vec3)player->getViewDirection(), vec4_white, 0.015f, false,
		0.0f, false);
}

void InteractorComponent::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(mouse_handle);
}
