// Demonstrates exporting C++ structs to UnigineScript using ExternClass. The MyVector
// struct is exposed with auto-generated getter/setter functions via addSetFunction and
// addGetFunction, which create property accessors from member variable pointers.

#include "ScriptStructure.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptStructure)

using namespace Unigine;

// ============================================================================
// Extern Struct - simple data container exported to script
// ============================================================================

// 4-component vector struct with public fields
struct MyVector
{
	MyVector()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
		, w(0.0f)
	{}

	float x;
	float y;
	float z;
	float w;
};

// ============================================================================
// Struct Registration
// ============================================================================

// Deferred registration: exposes MyVector with auto-generated property accessors
USCInterpreter structure_interpreter([]() {
	ExternClass<MyVector> *my_vector = MakeExternClass<MyVector>();
	my_vector->addConstructor();
	my_vector->addSetFunction("setX", &MyVector::x);
	my_vector->addGetFunction("getX", &MyVector::x);
	my_vector->addSetFunction("setY", &MyVector::y);
	my_vector->addGetFunction("getY", &MyVector::y);
	my_vector->addSetFunction("setZ", &MyVector::z);
	my_vector->addGetFunction("getZ", &MyVector::z);
	my_vector->addSetFunction("setW", &MyVector::w);
	my_vector->addGetFunction("getW", &MyVector::w);
	Interpreter::addExternClass("MyVector", my_vector);
});


// Configures on-screen console for displaying struct operation results
void ScriptStructure::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptStructure::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
