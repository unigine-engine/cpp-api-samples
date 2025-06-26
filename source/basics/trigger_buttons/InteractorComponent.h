#pragma once
#include "Interactable.h"

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

class InteractorComponent : public Unigine::ComponentBase
{
public:
	COMPONENT(InteractorComponent, Unigine::ComponentBase);
	PROP_NAME("Interactor");
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

	Unigine::PlayerPtr player;
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();
	Unigine::ObjectPtr obj;
	Interactable *comp_interactable = nullptr;

	Unigine::Input::MOUSE_HANDLE mouse_handle;
};
