#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineConsole.h>

class ConsoleSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ConsoleSample, ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, controllable_node)

private:
	void init();
	void shutdown();

	void command_callback(int argc, char **argv);
	void move_node_callback(int argc, char **argv);
};
