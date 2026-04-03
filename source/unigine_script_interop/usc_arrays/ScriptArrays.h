// Demonstrates C++ interop with UnigineScript arrays (ArrayVector, ArrayMap).
// Exports functions to manipulate indexed and key-value containers from script.
// Shows iteration patterns using forward/backward iterators for maps.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during array operations.
class ScriptArrays : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptArrays, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
