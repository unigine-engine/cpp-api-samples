#include "UnigineScriptsInterpreter.h"

InterpreterRegistrator *InterpreterRegistrator::get()
{
	static InterpreterRegistrator instanse;
	return &instanse;
}

void InterpreterRegistrator::initialize()
{
	Unigine::Log::message("InterpreterRegistrator::initialize()\n");
	invoker.run();
}
