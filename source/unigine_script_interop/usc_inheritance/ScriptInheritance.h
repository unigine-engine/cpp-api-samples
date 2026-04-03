// Demonstrates exporting C++ class hierarchies to UnigineScript.
// Shows inheritance chain (MyBaseClass -> MyNodeClass -> MyObjectClass) with
// polymorphic method calls and base class linkage via addBaseClass.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during inheritance demonstration.
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
