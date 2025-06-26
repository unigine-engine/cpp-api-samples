#include "ScriptStructure.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptStructure)

using namespace Unigine;

//////////////////////////////////////////////////////////////////////////
// Extern struct
//////////////////////////////////////////////////////////////////////////
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


USCInterpreter structure_interpreter([]() {
	// export extern struct
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


void ScriptStructure::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptStructure::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
