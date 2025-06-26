#pragma once

#include <UnigineComponentSystem.h>


class ScriptArrays : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptArrays, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
