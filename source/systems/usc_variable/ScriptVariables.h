#pragma once

#include <UnigineComponentSystem.h>


class ScriptVariables : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptVariables, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
