// Demonstrates custom type marshalling between C++ and UnigineScript.
// Shows TypeToVariable/VariableToType template specializations for automatic
// conversion between custom MyVector3 and built-in vec3 types.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during type conversion demonstration.
class ScriptTypes : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptTypes, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};
