#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include <UnigineComponentSystem.h>

// Demonstrates object lifetime management modes for nodes and widgets.
// LIFETIME_WORLD ties objects to the current world (destroyed on world change),
// while LIFETIME_ENGINE/LIFETIME_WINDOW keeps them alive across world transitions.
// Static pointers preserve references between world loads to show persistence behavior.
class LifetimeSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(LifetimeSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, node_to_spawn);

private:
	void init();
	void shutdown();

private:
	SampleDescriptionWindow description_window;

	static Unigine::NodePtr spawned_node;
	static Unigine::WidgetPtr spawned_widget;
};
