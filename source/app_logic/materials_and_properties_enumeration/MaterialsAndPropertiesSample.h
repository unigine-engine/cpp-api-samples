#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates Materials and Properties system introspection.
// Shows querying registered materials and properties from the engine,
// iterating their hierarchies, and accessing metadata (names, paths, children).
class MaterialsAndPropertiesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(MaterialsAndPropertiesSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates querying and printing engine properties and materials, "
							"including their names, file paths, and child counts, to the console.")

	COMPONENT_INIT(sample)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void sample();   // Queries and prints all properties and materials
	void shutdown(); // Restores console settings

private:
	bool is_console_onscreen{false}; // Saved console state to restore on shutdown
};
