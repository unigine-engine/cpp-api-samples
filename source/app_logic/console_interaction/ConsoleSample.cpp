// Demonstrates console system: ConsoleVariable for runtime-adjustable parameters,
// Console::addCommand for custom commands with argument parsing, and Console::run
// for executing commands programmatically.

#include "ConsoleSample.h"

REGISTER_COMPONENT(ConsoleSample)

using namespace Unigine;
using namespace Math;

// ConsoleVariable* macros create variables accessible from the console.
// Parameters: name, description, flags, default_value, min_value, max_value
// These persist globally and can be modified at runtime via console.
ConsoleVariableInt my_console_variable_int{"my_console_variable_int", "my_console_variable_int", 1,
	0, 0, 1000};
ConsoleVariableFloat my_console_variable_float{"my_console_variable_float",
	"my_console_variable_float", 1, 0.0f, 0.0f, 1.0f};
ConsoleVariableString my_console_variable_string{"my_console_variable_string",
	"my_console_variable_string", 1, nullptr};


void ConsoleSample::init()
{
	// Set initial values for console variables
	my_console_variable_float = 1.f;
	my_console_variable_int = 1;
	my_console_variable_string = "String";

	// Add a simple console command with a callback
	Console::addCommand("my_console_command", "my_console_command decription",
		MakeCallback(this, &ConsoleSample::command_callback));

	// Execute command directly from code
	Console::run("my_console_command \"Hello from C++\"");
	Console::setActive(true);

	// Add a command to move a node in the scene
	Console::addCommand("control_node",
		"With this command you can control node, pass desired position through the arguments",
		MakeCallback(this, &ConsoleSample::move_node_callback));

	if (!controllable_node)
	{
		Log::message("ConsoleSample::init(): No node was provided!\n");
	}

	Log::message("To move the node, you can use control_node command and 3 arguments to specify "
				 "node position");
}


void ConsoleSample::shutdown()
{
	// Remove commands and deactivate console
	Console::removeCommand("my_console_command");
	Console::removeCommand("control_node");
	Console::setActive(false);
}


// Callback for logging a command call and its arguments
void ConsoleSample::command_callback(int argc, char **argv)
{
	Log::message("my_console_command(): called\n");
	for (int i = 0; i < argc; i++)
	{
		Log::message("%d: %s\n", i, argv[i]);
	}
}

// Callback to move the node to coordinates provided via console
void ConsoleSample::move_node_callback(int argc, char **argv)
{
	if (!controllable_node)
		return;

	Vec3 node_position{};
	if (argc != 4)
	{
		Log::warning("control_node was called incorrectly, you need to pass 3 coordinates to move "
					 "the node\n");
		return;
	}

	const auto parse_arg = [this, &argv](int index) -> float {
		const char *a_value = argv[index];
		return Scalar(atof(a_value));
	};

	node_position.x = parse_arg(1);
	node_position.y = parse_arg(2);
	node_position.z = parse_arg(3);

	controllable_node->setWorldPosition(node_position);
}
