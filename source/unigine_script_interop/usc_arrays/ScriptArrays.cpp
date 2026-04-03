// Demonstrates C++ interop with UnigineScript arrays. Exports functions to manipulate
// ArrayVector (indexed) and ArrayMap (key-value) containers from script. Shows
// iteration patterns using forward/backward iterators for maps.

#include "ScriptArrays.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineConsole.h>
#include <UnigineInterface.h>
#include <UnigineWorld.h>

REGISTER_COMPONENT(ScriptArrays)

using namespace Unigine;


// ============================================================================
// ArrayVector Functions - indexed container access from script
// ============================================================================

// Sets element at specified index in script-passed ArrayVector
void my_array_vector_set(const Variable &id, int index, const Variable &v)
{
	ArrayVector vector = ArrayVector::get(Interpreter::get(), id);
	vector.set(index, v);
}

// Returns element at specified index from script-passed ArrayVector
Variable my_array_vector_get(const Variable &id, int index)
{
	ArrayVector vector = ArrayVector::get(Interpreter::get(), id);
	return vector.get(index);
}

// ============================================================================
// ArrayMap Functions - key-value container access from script
// ============================================================================

// Sets value for given key in script-passed ArrayMap
void my_array_map_set(const Variable &id, const Variable &key, const Variable &v)
{
	ArrayMap map = ArrayMap::get(Interpreter::get(), id);
	map.set(key, v);
}

// Returns value for given key from script-passed ArrayMap
Variable my_array_map_get(const Variable &id, const Variable &key)
{
	ArrayMap map = ArrayMap::get(Interpreter::get(), id);
	return map.get(key);
}

// ============================================================================
// Generator Functions - demonstrate container manipulation from C++
// ============================================================================

// Populates ArrayVector with squared values and a string element
void my_array_vector_generate(const Variable &id)
{
	ArrayVector vector = ArrayVector::get(Interpreter::get(), id);
	vector.clear();
	for (int i = 0; i < 4; i++)
		vector.append(Variable(i * i));
	vector.remove(0);
	vector.append(Variable("128"));
}

// Populates ArrayMap with squared key-value pairs and a string entry
void my_array_map_generate(const Variable &id)
{
	ArrayMap map = ArrayMap::get(Interpreter::get(), id);
	map.clear();
	for (int i = 0; i < 4; i++)
		map.append(Variable(i * i), Variable(i * i));
	map.remove(Variable(0));
	map.append(Variable(128), Variable("128"));
}

// ============================================================================
// Enumeration Functions - demonstrate iteration patterns
// ============================================================================

// Iterates ArrayVector by index and logs each element's type info
void my_array_vector_enumerate(const Variable &id)
{
	ArrayVector vector = ArrayVector::get(Interpreter::get(), id);
	for (int i = 0; i < vector.size(); i++)
		Log::message("%s %d: %s\n", sourse_str, i, vector.get(i).getTypeInfo().get());
}

// Iterates ArrayMap from first to last entry using forward iterator
void my_array_map_enumerate_forward(const Variable &id)
{
	ArrayMap map = ArrayMap::get(Interpreter::get(), id);
	ArrayMap::Iterator end = map.end();
	for (ArrayMap::Iterator it = map.begin(); it != end; ++it)
		Log::message("%s %d: %s\n", sourse_str, it.key().getInt(), it.get().getTypeInfo().get());
}

// Iterates ArrayMap from last to first entry using backward iterator
void my_array_map_enumerate_backward(const Variable &id)
{
	ArrayMap map = ArrayMap::get(Interpreter::get(), id);
	ArrayMap::Iterator end = map.end();
	for (ArrayMap::Iterator it = map.back(); it != end; --it)
		Log::message("%s %d: %s\n", sourse_str, it.key().getInt(), it.get().getTypeInfo().get());
}

// ============================================================================
// Function Registration - exports C++ functions callable from UnigineScript
// ============================================================================

// Deferred registration: lambda runs after engine init, before world init
USCInterpreter arrays_interpreter([]() {
	Interpreter::addExternFunction("my_array_vector_set",
		MakeExternFunction(&my_array_vector_set, "[]"));
	Interpreter::addExternFunction("my_array_vector_get",
		MakeExternFunction(&my_array_vector_get, "[]"));
	Interpreter::addExternFunction("my_array_map_set", MakeExternFunction(&my_array_map_set, "[]"));
	Interpreter::addExternFunction("my_array_map_get", MakeExternFunction(&my_array_map_get, "[]"));
	Interpreter::addExternFunction("my_array_vector_generate",
		MakeExternFunction(&my_array_vector_generate, "[]"));
	Interpreter::addExternFunction("my_array_map_generate",
		MakeExternFunction(&my_array_map_generate, "[]"));
	Interpreter::addExternFunction("my_array_vector_enumerate",
		MakeExternFunction(&my_array_vector_enumerate, "[]"));
	Interpreter::addExternFunction("my_array_map_enumerate_forward",
		MakeExternFunction(&my_array_map_enumerate_forward, "[]"));
	Interpreter::addExternFunction("my_array_map_enumerate_backward",
		MakeExternFunction(&my_array_map_enumerate_backward, "[]"));
});


// ============================================================================
// Component Lifecycle
// ============================================================================

// Configures on-screen console for displaying array operation results
void ScriptArrays::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenHeight(100);
	Console::setOnscreenTime(1000);
}

// Restores default console settings
void ScriptArrays::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
