// Demonstrates exporting C++ structs to UnigineScript via ExternClass.
// Shows auto-generated property accessors using addSetFunction/addGetFunction
// with member variable pointers instead of explicit getter/setter methods.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during struct demonstration.
class ScriptStructure : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptStructure, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};

