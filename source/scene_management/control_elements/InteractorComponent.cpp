// Player interaction system using raycasting. Pressing F initiates interaction with
// objects that have an Interactable component and matching intersection mask. Holding
// F continues the interaction, allowing continuous manipulation of levers and valves.

#include "InteractorComponent.h"

#include "UnigineVisualizer.h"

REGISTER_COMPONENT(InteractorComponent);

using namespace Unigine;
using namespace Math;

// Maximum raycast distance for detecting interactable objects
const float interaction_distance = 10;

// Mouse is grabbed and player reference is validated.
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

// Raycast is performed on F key; interaction continues while key is held.
void InteractorComponent::update()
{
	// Lock onto object when F is first pressed
	if (Input::isKeyDown(Input::KEY_F))
	{
		ivec2 mouse = Input::getMousePosition();
		Vec3 p0 = player->getWorldPosition();
		Vec3 p1 = p0
			+ Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y)) * interaction_distance;

		obj = World::getIntersection(p0, p1, interactable_intersection_mask.get(), intersection);
	}
	// Continue interaction while F is held
	if (Input::isKeyPressed(Input::KEY_F))
	{
		if (obj)
		{
			// Raycast with all masks to get accurate intersection point
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
	// Release object when F is released
	if (Input::isKeyUp(Input::KEY_F))
	{
		obj = nullptr;
	}
}

// View direction vector is visualized for debugging.
void InteractorComponent::post_update()
{
	Visualizer::renderVector(player->getWorldPosition(),
		player->getWorldPosition() + (Vec3)player->getViewDirection(), vec4_white, 0.015f, false,
		0.0f, false);
}

// Visualizer is disabled and mouse handle is restored.
void InteractorComponent::shutdown()
{
	Visualizer::setEnabled(false);
	Input::setMouseHandle(mouse_handle);
}
