#include <UnigineInit.h>
#include <UnigineEngine.h>

#include "AppSystemLogic.h"
#include "AppWorldLogic.h"
#include "utils/UnigineScriptsInterpreter.h"


#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
	// init engine
	Unigine::EnginePtr engine(argc, argv);

	InterpreterRegistrator::get()->initialize();

	// UnigineLogic
	AppSystemLogic system_logic;
	AppWorldLogic world_logic;

	// enter main loop
	engine->main(&system_logic, &world_logic);

	return 0;
}
