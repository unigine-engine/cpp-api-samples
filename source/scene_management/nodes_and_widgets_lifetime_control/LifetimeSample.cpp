// Demonstrates object lifetime management across world changes. Nodes and widgets
// can be set to LIFETIME_WORLD (destroyed on world change) or LIFETIME_ENGINE/WINDOW
// (persist until engine shutdown). Static pointers track objects across reloads.

#include "LifetimeSample.h"

REGISTER_COMPONENT(LifetimeSample);

using namespace Unigine;

Unigine::NodePtr LifetimeSample::spawned_node;
Unigine::WidgetPtr LifetimeSample::spawned_widget;

void LifetimeSample::init()
{
	// Static pointers persist across world reloads to demonstrate lifetime behavior.
	// On first run, both widget and node are created. On subsequent world loads,
	// their existence depends on the lifetime setting at the time of world change.

	if (!spawned_widget)
	{
		// LIFETIME_WORLD: widget is destroyed when the world is changed or reloaded.
		// LIFETIME_WINDOW: widget persists until the window is closed.
		WidgetButtonPtr button = WidgetButton::create("WIDGET FROM LIFETIME SAMPLE!");
		button->setFontSize(18);
		button->setLifetime(Widget::LIFETIME_WORLD);
		spawned_widget = button;
		Gui::getCurrent()->addChild(spawned_widget, Gui::ALIGN_OVERLAP | Gui::ALIGN_CENTER);

		Log::message("New widget created!\n");
	}
	if (!spawned_node)
	{
		// LIFETIME_WORLD: node is destroyed on world change.
		// LIFETIME_ENGINE: node persists for the entire engine session.
		spawned_node = World::loadNode(node_to_spawn);
		spawned_node->setWorldTransform(node->getWorldTransform());
		spawned_node->setLifetime(Node::LIFETIME_WORLD);

		Log::message("New node created!\n");
	}

	description_window.createWindow(Gui::ALIGN_RIGHT);

	int widget_cb_current = 0;
	switch (spawned_widget->getLifetime())
	{
	case Widget::LIFETIME_WORLD:
		widget_cb_current = 0;
		break;
	case Widget::LIFETIME_WINDOW:
		widget_cb_current = 1;
		break;
	default:
		break;
	}

	description_window.addSwitchParameter("Widget lifetime", "Widget lifetime", widget_cb_current, {"World", "Window"},
		[](int v) {
			spawned_widget->setLifetime(v == 0 ? Widget::LIFETIME_WORLD : Widget::LIFETIME_WINDOW);
		});

	int node_cb_current = 0;
	switch (spawned_node->getLifetime())
	{
	case Node::LIFETIME_WORLD:
		node_cb_current = 0;
		break;
	case Node::LIFETIME_ENGINE:
		node_cb_current = 1;
		break;
	default:
		break;
	}

	description_window.addSwitchParameter("Node lifetime", "Node lifetime", node_cb_current, {"World", "Engine"},
		[](int v) {
			spawned_node->setLifetime(v == 0 ? Node::LIFETIME_WORLD : Node::LIFETIME_ENGINE);
		});
}

void LifetimeSample::shutdown()
{
	description_window.shutdown();
}
