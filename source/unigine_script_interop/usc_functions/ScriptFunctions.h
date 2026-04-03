// Demonstrates exporting C++ functions to UnigineScript via Interpreter.
// Shows standalone functions with Variable polymorphism and class member functions
// exported as a library (singleton pattern).

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during function interop demonstration.
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
