// Demonstrates exporting C++ classes to UnigineScript. Shows two approaches:
// 1) Storing script user objects in C++ using Variable
// 2) Exporting C++ classes (MyExternObject) with constructors and methods via ExternClass

#include "ScriptClasses.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptClasses)

using namespace Unigine;
using namespace Math;


// ============================================================================
// User Class Pattern - storing script objects in C++
// ============================================================================

// Holds a reference to a script-created user object
Variable user_object;

// Stores a script object reference passed from UnigineScript
void SetMyUserObject(Variable &v)
{
	Log::message("%s SetMyUserObject(%s): called\n", sourse_str, v.getTypeName().get());
	user_object = v;
}

// Returns the stored script object reference back to UnigineScript
const Variable &GetMyUserObject()
{
	Log::message("%s GetMyUserObject(): called\n", sourse_str);
	return user_object;
}

// ============================================================================
// Extern Class Pattern - exposing C++ class to script
// ============================================================================

// Default constructor, initializes mass to zero
MyExternObject::MyExternObject()
	: mass(0.0f)
{
	Log::message("%s MyExternObject::MyExternObject(): called\n", sourse_str);
}

// Parameterized constructor with size and mass
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

// ============================================================================
// Alternative API - free functions wrapping class methods
// ============================================================================

// Factory function to create MyExternObject from script
MyExternObject *MakeMyExternObject(const vec3 &size, float mass)
{
	return new MyExternObject(size, mass);
}

// Destructor wrapper for script-side cleanup
void DeleteMyExternObject(MyExternObject *object)
{
	delete object;
}

// Free function wrapper for setSize method
void MyExternObjectSetSize(MyExternObject *object, const vec3 &size)
{
	object->setSize(size);
}

// Free function wrapper for getSize method
const vec3 &MyExternObjectGetSize(MyExternObject *object)
{
	return object->getSize();
}

// ============================================================================
// Function Registration
// ============================================================================

// Deferred registration: exports user object functions and MyExternObject class
USCInterpreter calsses_interpreter([]() {
	Interpreter::addExternFunction("SetMyUserObject", MakeExternFunction(&SetMyUserObject));
	Interpreter::addExternFunction("GetMyUserObject", MakeExternFunction(&GetMyUserObject));

	// Export MyExternObject class with constructors and methods
	ExternClass<MyExternObject> *my_object = MakeExternClass<MyExternObject>();
	my_object->addConstructor();
	my_object->addConstructor<const vec3 &, float>();
	my_object->addFunction("setSize", &MyExternObject::setSize);
	my_object->addFunction("getSize", &MyExternObject::getSize);
	my_object->addFunction("setMass", &MyExternObject::setMass);
	my_object->addFunction("getMass", &MyExternObject::getMass);
	Interpreter::addExternClass("MyExternObject", my_object);

	// Export free function wrappers as alternative API
	Interpreter::addExternFunction("DeleteMyExternObject",
		MakeExternFunction(&DeleteMyExternObject));
	Interpreter::addExternFunction("MakeMyExternObject", MakeExternFunction(&MakeMyExternObject));
	Interpreter::addExternFunction("MyExternObjectSetSize",
		MakeExternFunction(&MyExternObjectSetSize));
	Interpreter::addExternFunction("MyExternObjectGetSize",
		MakeExternFunction(&MyExternObjectGetSize));
});


// Configures on-screen console for displaying class operation results
void ScriptClasses::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptClasses::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
