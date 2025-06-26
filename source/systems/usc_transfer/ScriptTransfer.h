#pragma once

#include <UnigineComponentSystem.h>


class ScriptTransfer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptTransfer, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
