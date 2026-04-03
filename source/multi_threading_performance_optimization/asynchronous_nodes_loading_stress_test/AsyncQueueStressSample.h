#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Stress tests AsyncQueue by spawning many nodes in a background thread.
// Nodes are created and positioned off the main thread, then activated on main.
class AsyncQueueStressSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AsyncQueueStressSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates stress testing of asynchronous node loading. "
		"It uses AsyncQueue to spawn multiple nodes in a background thread, assigns them random positions, "
		"and activates them on the main thread while displaying progress in the UI.")

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Node that will be repeatedly spawned in the scene (set in the Editor)
	PROP_PARAM(File, node_to_spawn);

private:
	void init();     // Creates UI controls and enables profiler
	void update();   // Updates loaded node count display
	void shutdown(); // Disables profiler and closes UI

	// Loads a given number of nodes asynchronously
	void load_nodes(int num);

	// Counter of successfully loaded nodes
	Unigine::AtomicInt64 num_nodes_loaded;

	// Sample UI with description and controls
	Unigine::WidgetLabelPtr num_nodes_loaded_label;
	SampleDescriptionWindow sample_description_window;
};