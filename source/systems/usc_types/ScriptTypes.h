#pragma once

#include <UnigineComponentSystem.h>


class ScriptTypes : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptTypes, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
