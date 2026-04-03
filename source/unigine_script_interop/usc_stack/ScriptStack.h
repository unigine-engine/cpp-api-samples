// Demonstrates Interpreter stack access for variadic functions in UnigineScript.
// Shows how to pop Variable arguments from script stack using getStack/popStack.
// Implements printf-style formatting with %d, %f, %s specifiers.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during stack demonstration.
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
