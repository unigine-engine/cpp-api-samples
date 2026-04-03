// Player interaction controller that raycasts to find interactable objects.
// Pressing F initiates interaction; holding F continues it each frame.
// Attached to Player node to use view direction for raycasting.

#pragma once
#include "Interactable.h"

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

// Raycasts from player position and calls use() on intersected interactables.
class InteractorComponent : public Unigine::ComponentBase
{
public:
	COMPONENT(InteractorComponent, Unigine::ComponentBase);
	PROP_NAME("Interactor");
	// Bitmask filtering which objects can be interacted with
	PROP_PARAM(Mask, interactable_intersection_mask, 4);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void post_update();
	void shutdown();

	// Player reference for view direction and position
	Unigine::PlayerPtr player;
	// Reused intersection result to avoid allocations
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
	// Currently locked interaction target
	Unigine::ObjectPtr obj;
	// Cached interactable component from target object
	Interactable *comp_interactable = nullptr;

	// Saved mouse handle restored on shutdown
	Unigine::Input::MOUSE_HANDLE mouse_handle;
};
