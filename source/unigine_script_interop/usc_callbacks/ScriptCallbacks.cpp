// Demonstrates calling UnigineScript functions from C++ using Engine::runWorldFunction.
// The counter function is called each frame, and its return value is used to trigger
// additional script calls. Shows bidirectional C++/script communication.

#include "ScriptCallbacks.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptCallbacks)

using namespace Unigine;


// Wrapper that logs call info and invokes a script function by name with one argument
const Variable &runWorldFunction(const Variable &name, const Variable &v)
{
	Log::message("%s runWorldFunction(%s,%s): called\n", sourse_str, name.getTypeName().get(),
		v.getTypeName().get());

	return Engine::get()->runWorldFunction(name, v);
}

// Deferred registration: exposes runWorldFunction to UnigineScript
USCInterpreter callbacks_interpreter([]() {
	Interpreter::addExternFunction("runWorldFunction", MakeExternFunction(&runWorldFunction));
});


// Configures on-screen console for displaying script call results
void ScriptCallbacks::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Calls script "counter" function; on value 3, also retrieves and logs world path
void ScriptCallbacks::update()
{
	Variable ret = Engine::get()->runWorldFunction(Variable("counter"));
	if (ret.getInt() != -1)
		Log::message("%s counter is: %d\n", sourse_str, ret.getInt());
	if (ret.getInt() == 3)
	{
		Variable path = Engine::get()->runWorldFunction(Variable("engine.world.getPath"));
		Log::message("\n%s world path is: \"%s\"\n", sourse_str, path.getString());
	}
}

// Restores default console settings
void ScriptCallbacks::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
