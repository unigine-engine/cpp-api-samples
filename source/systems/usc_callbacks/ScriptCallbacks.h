#pragma once

#include <UnigineComponentSystem.h>


class ScriptCallbacks : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptCallbacks, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();
};
