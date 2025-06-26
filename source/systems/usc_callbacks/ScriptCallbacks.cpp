#include "ScriptCallbacks.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptCallbacks)

using namespace Unigine;


const Variable &runWorldFunction(const Variable &name, const Variable &v)
{
	Log::message("%s runWorldFunction(%s,%s): called\n", sourse_str, name.getTypeName().get(),
		v.getTypeName().get());

	return Engine::get()->runWorldFunction(name, v);
}

USCInterpreter callbacks_interpreter([]() {
	Interpreter::addExternFunction("runWorldFunction", MakeExternFunction(&runWorldFunction));
});


void ScriptCallbacks::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

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

void ScriptCallbacks::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
