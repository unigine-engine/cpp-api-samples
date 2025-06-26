#include "XmlSample.h"

#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

XmlPtr XmlSample::xml_create()
{
	XmlPtr xml = Xml::create("node");
	XmlPtr xml_0 = XmlPtr(xml->addChild("child", "arg=\"0\""));
	XmlPtr xml_1 = XmlPtr(xml_0->addChild("child", "arg=\"1\""));
	XmlPtr xml_2 = XmlPtr(xml_1->addChild("child", "arg=\"2\""));
	xml_2->setData("some data");
	xml->setAPIInterfaceOwner(false);
	return xml;
}

void XmlSample::xml_print(Unigine::XmlPtr xml, int offset)
{
	for (int i = 0; i < offset; i++)
	{
		Log::message(" ");
	}

	Log::message("%s: ", xml->getName());
	for (int i = 0; i < xml->getNumArgs(); ++i)
	{
		Log::message("%s=%s ", xml->getArgName(i), xml->getArgValue(i));
	}
	Log::message(": %s\n", xml->getData());


	for (int i = 0; i < xml->getNumChildren(); i++)
	{
		xml_print(XmlPtr(xml->getChild(i)), offset + 1);
	}
}


//------------------------Sample Logic-----------------------

REGISTER_COMPONENT(XmlSample)

void XmlSample::init()
{
	Console::setOnscreen(true);

	{
		Log::message("\n");
		// create the XML tree
		XmlPtr xml = xml_create();

		// print xml tree
		xml_print(xml, 0);
	}
}

void XmlSample::shutdown()
{
	Console::setOnscreen(false);
}
