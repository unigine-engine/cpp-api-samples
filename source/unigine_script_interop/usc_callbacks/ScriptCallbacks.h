// Demonstrates calling UnigineScript functions from C++ using Engine::runWorldFunction.
// Shows bidirectional communication: C++ calls script functions by name via Variable class.
// Registers a wrapper function exposing script invocation capability back to UnigineScript.

#pragma once

#include <UnigineComponentSystem.h>

// Calls UnigineScript functions each frame and displays results on-screen.
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
