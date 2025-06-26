#include "ScriptTypes.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptTypes)

using namespace Unigine;
using namespace Unigine::Math;

//////////////////////////////////////////////////////////////////////////
// User defined vector
//////////////////////////////////////////////////////////////////////////

class MyVector3
{
public:
	MyVector3()
		: x(0.0f)
		, y(0.0f)
		, z(0.0f)
	{}
	MyVector3(float x, float y, float z)
		: x(x)
		, y(y)
		, z(z)
	{}

	float X() const { return x; }
	float Y() const { return y; }
	float Z() const { return z; }
	float &X() { return x; }
	float &Y() { return y; }
	float &Z() { return z; }

private:
	float x;
	float y;
	float z;
};

MyVector3 my_add(const MyVector3 &v0, const MyVector3 &v1)
{
	return MyVector3(v0.X() + v1.X(), v0.Y() + v1.Y(), v0.Z() + v1.Z());
}

MyVector3 my_sub(MyVector3 v0, MyVector3 v1)
{
	return MyVector3(v0.X() - v1.X(), v0.Y() - v1.Y(), v0.Z() - v1.Z());
}

float my_vec_dot(MyVector3 v0, MyVector3 v1)
{
	return v0.X() * v1.X() + v0.Y() * v1.Y() + v0.Z() * v1.Z();
}

//////////////////////////////////////////////////////////////////////////
// User data type conversion
//////////////////////////////////////////////////////////////////////////

namespace Unigine
{
template<>
struct TypeToVariable<MyVector3>
{
	TypeToVariable(void *i, const MyVector3 &v) { value.setVec3(vec3(v.X(), v.Y(), v.Z())); }
	Variable value;
};

template<>
struct VariableToType<MyVector3>
{
	VariableToType(void *i, const Variable &variable)
	{
		vec3 v = variable.getVec3();
		value = MyVector3(v.x, v.y, v.z);
	}
	MyVector3 value;
};

template<>
struct VariableToType<const MyVector3 &>
{
	VariableToType(void *i, const Variable &variable)
	{
		vec3 v = variable.getVec3();
		value = MyVector3(v.x, v.y, v.z);
	}
	MyVector3 value;
};
} // namespace Unigine


USCInterpreter types_interpreter([]() {
	// export class member functions
	Interpreter::addExternFunction("my_add", MakeExternFunction(my_add));
	Interpreter::addExternFunction("my_sub", MakeExternFunction(my_sub));
	Interpreter::addExternFunction("my_vec_dot", MakeExternFunction(my_vec_dot));
});


void ScriptTypes::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptTypes::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
