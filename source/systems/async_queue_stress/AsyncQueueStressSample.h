#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class AsyncQueueStressSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(AsyncQueueStressSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, node_to_spawn);

private:
	void init();
	void update();
	void shutdown();

	void load_nodes(int num);

	Unigine::AtomicInt64 num_nodes_loaded;
	Unigine::WidgetLabelPtr num_nodes_loaded_label;

	SampleDescriptionWindow sample_description_window;
};