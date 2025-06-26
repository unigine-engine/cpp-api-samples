#pragma once

#include <UnigineComponentSystem.h>

class ScriptFunctions : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptFunctions, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
