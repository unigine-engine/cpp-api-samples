// Demonstrates Variable class type system for C++/UnigineScript interop.
// Tests all supported primitive and vector types by passing them to script
// function and logging the returned type information.

#include "ScriptVariables.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptVariables)

using namespace Unigine;
using namespace Unigine::Math;

// ============================================================================
// Type Info Function - logs Variable type and value
// ============================================================================

// Inspects Variable type and logs its value in appropriate format
void my_typeinfo(const Variable &v)
{
	switch (v.getType())
	{
	case Variable::INT: Log::message("%s my_typeinfo(): int: %d\n", sourse_str, v.getInt()); break;
	case Variable::LONG:
		Log::message("%s my_typeinfo(): long: %lld\n", sourse_str, v.getLong());
		break;
	case Variable::FLOAT:
		Log::message("%s my_typeinfo(): float: %g\n", sourse_str, v.getFloat());
		break;
	case Variable::DOUBLE:
		Log::message("%s my_typeinfo(): double: %g\n", sourse_str, v.getDouble());
		break;
	case Variable::VEC3:
		Log::message("%s my_typeinfo(): vec3: %g %g %g\n", sourse_str, v.getVec3().x, v.getVec3().y,
			v.getVec3().z);
		break;
	case Variable::VEC4:
		Log::message("%s my_typeinfo(): vec4: %g %g %g %g\n", sourse_str, v.getVec4().x,
			v.getVec4().y, v.getVec4().z, v.getVec4().w);
		break;
	case Variable::DVEC3:
		Log::message("%s my_typeinfo(): dvec3: %g %g %g\n", sourse_str, v.getDVec3().x,
			v.getDVec3().y, v.getDVec3().z);
		break;
	case Variable::DVEC4:
		Log::message("%s my_typeinfo(): dvec4: %g %g %g %g\n", sourse_str, v.getDVec4().x,
			v.getDVec4().y, v.getDVec4().z, v.getDVec4().w);
		break;
	case Variable::IVEC3:
		Log::message("%s my_typeinfo(): ivec3: %d %d %d\n", sourse_str, v.getIVec3().x,
			v.getIVec3().y, v.getIVec3().z);
		break;
	case Variable::IVEC4:
		Log::message("%s my_typeinfo(): ivec4: %d %d %d %d\n", sourse_str, v.getIVec4().x,
			v.getIVec4().y, v.getIVec4().z, v.getIVec4().w);
		break;
	}
}

// ============================================================================
// Component Lifecycle - tests all Variable types
// ============================================================================

// Creates Variables of each type and passes to script for round-trip test
void ScriptVariables::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);

	Engine *engine = Engine::get();
	Variable value, ret;

	// Get script function ID for repeated calls
	Variable my_typeinfo_id = Variable(engine->getWorldFunction("my_typeinfo", 1));

	// Test integer type
	value.setInt(13);
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test long long type
	value.setLong(13LL);
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test float type
	value.setFloat(13.0f);
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test double type
	value.setDouble(13.0);
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test vec3 type (3-component float vector)
	value.setVec3(vec3(1.0f, 2.0f, 3.0f));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test vec4 type (4-component float vector)
	value.setVec4(vec4(1.0f, 2.0f, 3.0f, 4.0f));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test dvec3 type (3-component double vector)
	value.setDVec3(dvec3(1.0, 2.0, 3.0));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test dvec4 type (4-component double vector)
	value.setDVec4(dvec4(1.0, 2.0, 3.0, 4.0));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test ivec3 type (3-component integer vector)
	value.setIVec3(ivec3(1, 2, 3));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);

	// Test ivec4 type (4-component integer vector)
	value.setIVec4(ivec4(1, 2, 3, 4));
	ret = engine->runWorldFunction(my_typeinfo_id, value);
	my_typeinfo(ret);
}

// Restores default console settings
void ScriptVariables::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
