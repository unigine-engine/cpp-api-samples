// Demonstrates Unigine::Xml API for creating and traversing XML structures.
// Shows building a hierarchical tree with nested nodes, attributes, and data.
// Recursive print function displays structure with depth-based indentation.

#include "XmlSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

// Create a hierarchical XML structure with nested nodes and attributes
XmlPtr XmlSample::xml_create()
{
	XmlPtr xml = Xml::create("node");
	XmlPtr xml_0 = XmlPtr(xml->addChild("child", "arg=\"0\""));
	XmlPtr xml_1 = XmlPtr(xml_0->addChild("child", "arg=\"1\""));
	XmlPtr xml_2 = XmlPtr(xml_1->addChild("child", "arg=\"2\""));
	xml_2->setData("some data");
	return xml;
}

// Recursively print XML structure with indentation for hierarchy visualization
void XmlSample::xml_print(Unigine::XmlPtr xml, int offset)
{
	// Create indentation based on node depth
	for (int i = 0; i < offset; i++)
	{
		Log::message(" ");
	}

	// Print node name, attributes, and data
	Log::message("%s: ", xml->getName());
	for (int i = 0; i < xml->getNumArgs(); ++i)
	{
		Log::message("%s=%s ", xml->getArgName(i), xml->getArgValue(i));
	}
	Log::message(": %s\n", xml->getData());

	// Recursively print all child nodes
	for (int i = 0; i < xml->getNumChildren(); i++)
	{
		xml_print(XmlPtr(xml->getChild(i)), offset + 1);
	}
}


// Sample Logic

REGISTER_COMPONENT(XmlSample)

void XmlSample::init()
{
	Console::setOnscreen(true);

	{
		Log::message("\n");
		// Create XML structure and print its hierarchical content
		XmlPtr xml = xml_create();
		xml_print(xml, 0);
	}
}

void XmlSample::shutdown()
{
	Console::setOnscreen(false);
}
