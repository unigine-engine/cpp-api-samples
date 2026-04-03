// Demonstrates custom type marshalling between C++ and UnigineScript. The MyVector3 class
// is made compatible with script via TypeToVariable and VariableToType template specializations,
// which convert between MyVector3 and the built-in vec3 type for seamless interop.

#include "ScriptTypes.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptTypes)

using namespace Unigine;
using namespace Unigine::Math;

// ============================================================================
// Custom Vector Class - not directly compatible with UnigineScript
// ============================================================================

// Custom 3D vector with private fields and accessor methods
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

// Vector addition using custom type
MyVector3 my_add(const MyVector3 &v0, const MyVector3 &v1)
{
	return MyVector3(v0.X() + v1.X(), v0.Y() + v1.Y(), v0.Z() + v1.Z());
}

// Vector subtraction using custom type
MyVector3 my_sub(MyVector3 v0, MyVector3 v1)
{
	return MyVector3(v0.X() - v1.X(), v0.Y() - v1.Y(), v0.Z() - v1.Z());
}

// Dot product using custom type
float my_vec_dot(MyVector3 v0, MyVector3 v1)
{
	return v0.X() * v1.X() + v0.Y() * v1.Y() + v0.Z() * v1.Z();
}

// ============================================================================
// Type Conversion Templates - enable MyVector3 <-> vec3 marshalling
// ============================================================================

namespace Unigine
{
// Converts MyVector3 to Variable (for returning to script)
template<>
struct TypeToVariable<MyVector3>
{
	TypeToVariable(void *i, const MyVector3 &v) { value.setVec3(vec3(v.X(), v.Y(), v.Z())); }
	Variable value;
};

// Converts Variable to MyVector3 (for receiving from script, by value)
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

// Converts Variable to MyVector3 (for receiving from script, by const reference)
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

// ============================================================================
// Function Registration
// ============================================================================

// Deferred registration: exports vector math functions using custom type
USCInterpreter types_interpreter([]() {
	Interpreter::addExternFunction("my_add", MakeExternFunction(my_add));
	Interpreter::addExternFunction("my_sub", MakeExternFunction(my_sub));
	Interpreter::addExternFunction("my_vec_dot", MakeExternFunction(my_vec_dot));
});


// Configures on-screen console for displaying type conversion results
void ScriptTypes::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptTypes::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
