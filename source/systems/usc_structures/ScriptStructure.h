#pragma once

#include <UnigineComponentSystem.h>

class ScriptStructure : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptStructure, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};

