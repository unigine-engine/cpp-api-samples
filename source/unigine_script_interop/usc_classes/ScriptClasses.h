// Demonstrates exporting C++ classes to UnigineScript via ExternClass.
// Shows two patterns: storing script user objects in C++ and exposing C++ classes
// with constructors, methods, and properties callable from script.

#pragma once

#include <UnigineComponentSystem.h>

// Configures console for on-screen output during class interop demonstration.
class ScriptClasses : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ScriptClasses, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void shutdown();
};


// Example C++ class exported to UnigineScript. Inherits Base for reference counting.
class MyExternObject : public Unigine::Base
{
public:
	MyExternObject();
	MyExternObject(const Unigine::Math::vec3 &size, float mass);
	~MyExternObject();

	// Size property accessors
	void setSize(const Unigine::Math::vec3 &s);
	const Unigine::Math::vec3 &getSize() const { return size; }

	// Mass property accessors
	void setMass(float m);
	float getMass() const { return mass; }

private:
	Unigine::Math::vec3 size;
	float mass;
};
