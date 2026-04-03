// Demonstrates passing UNIGINE objects between C++ and UnigineScript.
// Compares three approaches: direct Ptr, Variable with setImage/getImage,
// and TypeToVariable/VariableToType templates for automatic conversion.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during object transfer demonstration.
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
