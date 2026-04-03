#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates XML API for hierarchical data handling.
// Shows creating XML nodes with attributes, child elements, and text data.
// Includes recursive tree traversal for formatted console output.
class XmlSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(XmlSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates creating a hierarchical XML structure with attributes and data, "
							"and recursively printing its contents to the console.")

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void shutdown();

	// Create and populate XML structure
	Unigine::XmlPtr xml_create();

	// Print XML content with formatting
	void xml_print(Unigine::XmlPtr xml, int offset = 0);
};
