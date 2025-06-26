#include "ScriptInheritance.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptInheritance)

using namespace Unigine;

//////////////////////////////////////////////////////////////////////////
// User defined class
//////////////////////////////////////////////////////////////////////////

class MyBaseClass
{
public:
	MyBaseClass() { Log::message("%s MyBaseClass::MyBaseClass(): called\n", sourse_str); }
	virtual ~MyBaseClass() { Log::message("%s MyBaseClass::~MyBaseClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyBaseClass::function(): called\n", sourse_str); }
	virtual const char *getName() = 0;
};

class MyNodeClass : public MyBaseClass
{
public:
	MyNodeClass() { Log::message("%s MyNodeClass::MyNodeClass(): called\n", sourse_str); }
	virtual ~MyNodeClass() { Log::message("%s MyNodeClass::~MyNodeClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyNodeClass::function(): called\n", sourse_str); }
	virtual const char *getName() { return "MyNodeClass"; }
};

class MyObjectClass : public MyNodeClass
{
public:
	MyObjectClass() { Log::message("%s MyObjectClass::MyObjectClass(): called\n", sourse_str); }
	virtual ~MyObjectClass() { Log::message("%s MyObjectClass::~MyObjectClass(): called\n", sourse_str); }

	void function() { Log::message("%s MyObjectClass::function(): called\n", sourse_str); }
	virtual const char *getName() { return "MyObjectClass"; }
};


USCInterpreter inheritance_interpreter([]() {
	// export classes
	ExternClass<MyBaseClass> *my_base = MakeExternClass<MyBaseClass>();
	my_base->addFunction("function", &MyBaseClass::function);
	my_base->addFunction("getName", &MyBaseClass::getName);
	Interpreter::addExternClass("MyBaseClass", my_base);

	ExternClass<MyNodeClass> *my_node = MakeExternClass<MyNodeClass>();
	my_node->addConstructor();
	my_node->addFunction("function", &MyNodeClass::function);
	my_node->addBaseClass(my_base);
	Interpreter::addExternClass("MyNodeClass", my_node);

	ExternClass<MyObjectClass> *my_object = MakeExternClass<MyObjectClass>();
	my_object->addConstructor();
	my_object->addFunction("function", &MyObjectClass::function);
	my_object->addBaseClass(my_node);
	Interpreter::addExternClass("MyObjectClass", my_object);
});


void ScriptInheritance::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptInheritance::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
