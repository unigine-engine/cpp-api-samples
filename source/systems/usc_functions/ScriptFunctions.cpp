#include "ScriptFunctions.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptFunctions)

using namespace Unigine;
using namespace Math;

//////////////////////////////////////////////////////////////////////////
// User defined functions
//////////////////////////////////////////////////////////////////////////

Variable my_sum(const Variable &v0, const Variable &v1)
{
	if (v0.getType() == Variable::INT && v1.getType() == Variable::INT)
	{
		Log::message("%s my_sum(%d,%d): called\n", sourse_str, v0.getInt(), v1.getInt());
		return Variable(v0.getInt() + v1.getInt());
	}

	if (v0.getType() == Variable::STRING && v1.getType() == Variable::STRING)
	{
		Log::message("%s my_sum(%s,%s): called\n", sourse_str, v0.getString(), v1.getString());
		return Variable((String(v0.getString()) + "+" + String(v1.getString())).get());
	}

	Log::message("%s my_sum(%s,%s): called\n", sourse_str, v0.getTypeName().get(),
		v1.getTypeName().get());
	return Variable("unknown");
}

float my_mul(float a, float b)
{
	Log::message("%s my_mul(%g,%g): called\n", sourse_str, a, b);
	return a * b;
}

float my_dot(const vec3 &v0, const vec3 &v1)
{
	Log::message("%s my_dot((%g,%g,%g),(%g,%g,%g)): called\n", sourse_str, v0.x, v0.y, v0.z, v1.x,
		v1.y, v1.z);
	return dot(v0, v1);
}

//////////////////////////////////////////////////////////////////////////
// User defined class member functions
//////////////////////////////////////////////////////////////////////////

class MyApplication
{
public:
	MyApplication()
		: seed(1)
	{}

	void init(int s = 1)
	{
		Log::message("%s MyApplication::init(%d) called\n", sourse_str, s);
		seed = s;
	}

	void shutdown()
	{
		Log::message("%s MyApplication::shutdown() called\n", sourse_str);
		seed = 1;
	}

	int update()
	{
		seed = (seed * 3877 + 29573) % 139968;
		return seed;
	}

	int get() const { return seed; }

private:
	int seed;
} my_application;


USCInterpreter functions_interpreter([]() {
	// export functions
	Interpreter::addExternFunction("my_sum", MakeExternFunction(&my_sum, ",1"));
	Interpreter::addExternFunction("my_mul", MakeExternFunction(&my_mul));
	Interpreter::addExternFunction("my_dot", MakeExternFunction(&my_dot));

	// export class member functions
	Interpreter::addExternLibrary("my_application");
	Interpreter::addExternFunction("my_application.init",
		MakeExternObjectFunction(&my_application, &MyApplication::init, "1"));
	Interpreter::addExternFunction("my_application.shutdown",
		MakeExternObjectFunction(&my_application, &MyApplication::shutdown));
	Interpreter::addExternFunction("my_application.update",
		MakeExternObjectFunction(&my_application, &MyApplication::update));
	Interpreter::addExternFunction("my_application.get",
		MakeExternObjectFunction(&my_application, &MyApplication::get));
});


void ScriptFunctions::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptFunctions::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
