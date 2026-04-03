// Demonstrates exporting C++ class hierarchies to UnigineScript. Three classes
// (MyBaseClass -> MyNodeClass -> MyObjectClass) are exported with inheritance
// relationships via addBaseClass, enabling polymorphic script usage.

#include "ScriptInheritance.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptInheritance)

using namespace Unigine;

// ============================================================================
// Class Hierarchy - demonstrates inheritance export to script
// ============================================================================

// Abstract base class with virtual getName method
class MyBaseClass
{
public:
	MyBaseClass() { Log::message("%s MyBaseClass::MyBaseClass(): called\n", sourse_str); }
	virtual ~MyBaseClass() { Log::message("%s MyBaseClass::~MyBaseClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyBaseClass::function(): called\n", sourse_str); }
	virtual const char *getName() = 0;
};

// First derived class, provides getName implementation
class MyNodeClass : public MyBaseClass
{
public:
	MyNodeClass() { Log::message("%s MyNodeClass::MyNodeClass(): called\n", sourse_str); }
	virtual ~MyNodeClass() { Log::message("%s MyNodeClass::~MyNodeClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyNodeClass::function(): called\n", sourse_str); }
	virtual const char *getName() { return "MyNodeClass"; }
};

// Second derived class, overrides getName with different value
class MyObjectClass : public MyNodeClass
{
public:
	MyObjectClass() { Log::message("%s MyObjectClass::MyObjectClass(): called\n", sourse_str); }
	virtual ~MyObjectClass() { Log::message("%s MyObjectClass::~MyObjectClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyObjectClass::function(): called\n", sourse_str); }
	virtual const char *getName() { return "MyObjectClass"; }
};

// ============================================================================
// Class Registration
// ============================================================================

// Deferred registration: exports class hierarchy with inheritance relationships
USCInterpreter inheritance_interpreter([]() {
	// Export abstract base class (no constructor - cannot be instantiated)

	ExternClass<MyBaseClass> *my_base = MakeExternClass<MyBaseClass>();
	my_base->addFunction("function", &MyBaseClass::function);
	my_base->addFunction("getName", &MyBaseClass::getName);
	Interpreter::addExternClass("MyBaseClass", my_base);

	// Export MyNodeClass with constructor and base class link
	ExternClass<MyNodeClass> *my_node = MakeExternClass<MyNodeClass>();
	my_node->addConstructor();
	my_node->addFunction("function", &MyNodeClass::function);
	my_node->addBaseClass(my_base);
	Interpreter::addExternClass("MyNodeClass", my_node);

	// Export MyObjectClass with constructor and base class link
	ExternClass<MyObjectClass> *my_object = MakeExternClass<MyObjectClass>();
	my_object->addConstructor();
	my_object->addFunction("function", &MyObjectClass::function);
	my_object->addBaseClass(my_node);
	Interpreter::addExternClass("MyObjectClass", my_object);
});


// Configures on-screen console for displaying inheritance demo results
void ScriptInheritance::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptInheritance::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
