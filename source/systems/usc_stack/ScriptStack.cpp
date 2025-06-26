#include "ScriptStack.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptStack)

using namespace Unigine;

//////////////////////////////////////////////////////////////////////////
// Format function
//////////////////////////////////////////////////////////////////////////

String my_format(const char *format)
{
	String ret;

	const char *s = format;

	while (*s)
	{
		// format symbol
		if (*s == '%')
		{
			s++;

			// format symbol
			if (*s == '%')
			{
				ret += *s++;
				continue;
			}

			// check stack depth
			if (Interpreter::getStack() < 1)
				Interpreter::error("my_format(): stack underflow\n");

			// pop a variable from the stack
			Variable v = Interpreter::popStack();

			// integer
			if (*s == 'd' || *s == 'i')
			{
				ret += String::format("%d", v.getInt());
				s++;
			}

			// float
			else if (*s == 'f')
			{
				ret += String::format("%f", v.getFloat());
				s++;
			}

			// string
			else if (*s == 's')
			{
				ret += String::format("%s", v.getString());
				s++;
			}

			// unknown format
			else
				Interpreter::error("my_format(): unknown format %c\n", *s);
		}

		// copy symbol
		else
			ret += *s++;
	}

	return ret;
}


USCInterpreter stack_interpreter([]() {
	// export format function
	Interpreter::addExternFunction("my_format", MakeExternFunction(&my_format, ",..."));
});


void ScriptStack::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

void ScriptStack::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
