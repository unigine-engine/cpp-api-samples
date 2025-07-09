#include "AppSystemLogic.h"
#include <UnigineComponentSystem.h>

// System logic, it exists during the application life cycle.
// These methods are called right after corresponding system script's (UnigineScript) methods.

AppSystemLogic::AppSystemLogic()
{
}

AppSystemLogic::~AppSystemLogic()
{
}

int AppSystemLogic::init()
{
	// Write here code to be called on engine initialization.
	Unigine::Engine::get()->setBackgroundUpdate(
		Unigine::Engine::BACKGROUND_UPDATE_RENDER_NON_MINIMIZED);

	Unigine::World::loadWorld("cpp_samples/cpp_samples");

	Unigine::ComponentSystem::get()->initialize();
	Unigine::Input::setMouseHandle(Unigine::Input::MOUSE_HANDLE_USER);

	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////
int AppSystemLogic::update()
{
	// Write here code to be called before updating each render frame.
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppSystemLogic::shutdown()
{
	// Write here code to be called on engine shutdown.
	return 1;
}
