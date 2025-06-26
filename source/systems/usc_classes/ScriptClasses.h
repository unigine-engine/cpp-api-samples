#pragma once

#include <UnigineComponentSystem.h>


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


// TODO Inhereted from Unigine::Base for what???
class MyExternObject : public Unigine::Base
{
public:
	MyExternObject();
	MyExternObject(const Unigine::Math::vec3 &size, float mass);
	~MyExternObject();

	// size
	void setSize(const Unigine::Math::vec3 &s);
	const Unigine::Math::vec3 &getSize() const { return size; }

	// mass
	void setMass(float m);
	float getMass() const { return mass; }

private:
	Unigine::Math::vec3 size;
	float mass;
};
