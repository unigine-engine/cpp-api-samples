// Demonstrates Variable class type system for C++/UnigineScript interop.
// Shows wrapping primitives (int, long, float, double) and vectors (vec3, vec4,
// dvec3, dvec4, ivec3, ivec4) in Variable objects for script function calls.

#pragma once

#include <UnigineComponentSystem.h>

// Tests Variable type handling by passing various types to script and back.
class ScriptVariables : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptVariables, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
