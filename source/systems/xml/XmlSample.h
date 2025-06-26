#pragma once

#include <UnigineComponentSystem.h>

class XmlSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(XmlSample, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void shutdown();

	Unigine::XmlPtr xml_create();
	void xml_print(Unigine::XmlPtr xml, int offset = 0);
};
