#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineConsole.h>

// Demonstrates console integration: custom variables, commands, and callbacks.
// ConsoleVariable types (Int, Float, String) are exposed to the in-game console.
// Custom commands are registered with callbacks that receive argc/argv arguments.
class ConsoleSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ConsoleSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates console integration by registering custom "
			"variables and commands, executing callbacks, and allowing control of a scene node via console arguments.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

	// Node that can be controlled via console command (set in the Editor)
	PROP_PARAM(Node, controllable_node)

private:
	void init();
	void shutdown();

	// Callbacks for console commands
	void command_callback(int argc, char **argv);
	void move_node_callback(int argc, char **argv);
};
