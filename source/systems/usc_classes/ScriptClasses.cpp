#include "ScriptClasses.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptClasses)

using namespace Unigine;
using namespace Math;


//////////////////////////////////////////////////////////////////////////
// User class
//////////////////////////////////////////////////////////////////////////

Variable user_object;

void SetMyUserObject(Variable &v)
{
	Log::message("%s SetMyUserObject(%s): called\n", sourse_str, v.getTypeName().get());
	user_object = v;
}

const Variable &GetMyUserObject()
{
	Log::message("%s GetMyUserObject(): called\n", sourse_str);
	return user_object;
}

//////////////////////////////////////////////////////////////////////////
// Extern class
//////////////////////////////////////////////////////////////////////////

MyExternObject::MyExternObject()
	: mass(0.0f)
{
	Log::message("%s MyExternObject::MyExternObject(): called\n", sourse_str);
}

MyExternObject::MyExternObject(const vec3 &size, float mass)
	: size(size)
	, mass(mass)
{
	Log::message("%s MyExternObject::MyExternObject((%g,%g,%g),%g): called\n", sourse_str, size.x,
		size.y, size.z, mass);
}

MyExternObject::~MyExternObject()
{
	Log::message("%s MyExternObject::~MyExternObject(): called\n", sourse_str);
}

void MyExternObject::setSize(const vec3 &s)
{
	Log::message("%s MyExternObject::setSize((%g,%g,%g)): called\n", sourse_str, s.x, s.y, s.z);
	size = s;
}

void MyExternObject::setMass(float m)
{
	Log::message("%s MyExternObject::setMass(%g): called\n", sourse_str, m);
	mass = m;
}


MyExternObject *MakeMyExternObject(const vec3 &size, float mass)
{
	return new MyExternObject(size, mass);
}

void DeleteMyExternObject(MyExternObject *object)
{
	delete object;
}

void MyExternObjectSetSize(MyExternObject *object, const vec3 &size)
{
	object->setSize(size);
}

const vec3 &MyExternObjectGetSize(MyExternObject *object)
{
	return object->getSize();
}


USCInterpreter calsses_interpreter([]() {
	Interpreter::addExternFunction("SetMyUserObject", MakeExternFunction(&SetMyUserObject));
	Interpreter::addExternFunction("GetMyUserObject", MakeExternFunction(&GetMyUserObject));

	// export extern class
	ExternClass<MyExternObject> *my_object = MakeExternClass<MyExternObject>();
	my_object->addConstructor();
	my_object->addConstructor<const vec3 &, float>();
	my_object->addFunction("setSize", &MyExternObject::setSize);
	my_object->addFunction("getSize", &MyExternObject::getSize);
	my_object->addFunction("setMass", &MyExternObject::setMass);
	my_object->addFunction("getMass", &MyExternObject::getMass);
	Interpreter::addExternClass("MyExternObject", my_object);

	// export extern class functions
	Interpreter::addExternFunction("DeleteMyExternObject",
		MakeExternFunction(&DeleteMyExternObject));
	Interpreter::addExternFunction("MakeMyExternObject", MakeExternFunction(&MakeMyExternObject));
	Interpreter::addExternFunction("MyExternObjectSetSize",
		MakeExternFunction(&MyExternObjectSetSize));
	Interpreter::addExternFunction("MyExternObjectGetSize",
		MakeExternFunction(&MyExternObjectGetSize));
});


void ScriptClasses::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptClasses::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
