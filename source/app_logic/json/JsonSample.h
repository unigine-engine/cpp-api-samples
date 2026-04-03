#pragma once
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates JSON API for data serialization.
// Shows creating hierarchical JSON structures, adding typed values,
// and recursive tree traversal for pretty-printing.
class JsonSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(JsonSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates creating a JSON tree, recursively traversing it, "
							"and printing a formatted representation to the console.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void shutdown();

	// Creates a sample JSON object (root node + children)
	Unigine::JsonPtr json_create();

	// Prints entire JSON tree
	void json_print(const Unigine::JsonPtr &json);

	// Recursive function: prints a single node (object, array, value, etc.)
	void json_print_node(const Unigine::JsonPtr &json, int offset = 0, int index = 0,
		bool print_name = true);

	// Utility function: prints a string with indentation + optional comma
	void print_string_offset(const Unigine::String &value, int offset, bool print_comma = false);

private:
	bool is_console_onscreen{false}; // Saved console state to restore on shutdown
};
