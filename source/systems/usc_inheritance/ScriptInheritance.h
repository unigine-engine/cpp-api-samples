#pragma once

#include <UnigineComponentSystem.h>


class ScriptInheritance : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptInheritance, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
