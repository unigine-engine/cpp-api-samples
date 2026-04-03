// Demonstrates Interpreter stack access for variadic functions. The my_format
// function implements printf-style formatting by popping Variable arguments
// from the script stack based on format specifiers (%d, %f, %s).

#include "ScriptStack.h"

#include "../../utils/UnigineScriptsInterpreter.h"

#include <UnigineInterface.h>
#include <UnigineWorld.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ScriptStack)

using namespace Unigine;

// ============================================================================
// Variadic Format Function - uses script stack for variable arguments
// ============================================================================

// Printf-style formatter that pops arguments from Interpreter stack
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

// ============================================================================
// Function Registration
// ============================================================================

// Deferred registration: ",..." signature indicates variadic stack-based arguments
USCInterpreter stack_interpreter([]() {
	Interpreter::addExternFunction("my_format", MakeExternFunction(&my_format, ",..."));
});


// Configures on-screen console for displaying format results
void ScriptStack::init()
{
	Console::setOnscreen(true);
	Console::setOnscreenFontSize(15);
	Console::setOnscreenTime(1000);
	Console::setOnscreenHeight(100);
}

// Restores default console settings
void ScriptStack::shutdown()
{
	Console::setOnscreen(false);
	Console::setOnscreenHeight(30);
}
