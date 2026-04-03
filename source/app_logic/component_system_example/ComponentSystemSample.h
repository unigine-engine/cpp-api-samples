#pragma once
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class Pawn;

// Demonstrates the Component System architecture and common patterns.
// Shows runtime component creation, property-based component attachment,
// and inter-component communication.
// Implements a mini-game: spinners spawn projectiles that damage the pawn.
class ComponentSystemSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ComponentSystemSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void my_pawn_destroyed() { my_pawn = nullptr; }
	Unigine::ObjectMeshDynamicPtr create_box(const Unigine::Math::Mat4 &transform,
		const Unigine::Math::vec3 &size);
	void init_gui();

private:
	Pawn *my_pawn;
	float time;

	Unigine::WidgetLabelPtr label;
};
