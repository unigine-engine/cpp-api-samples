#pragma once

#include <UnigineComponentSystem.h>


class ScriptStack : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptStack, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
