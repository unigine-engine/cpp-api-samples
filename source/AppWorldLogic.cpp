#include "AppWorldLogic.h"
#include <UnigineWorld.h>
#include <UnigineDir.h>
#include <UnigineFileSystem.h>
#include <UnigineWidgets.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;


AppWorldLogic::AppWorldLogic()
{
	
}

AppWorldLogic::~AppWorldLogic()
{
}

int AppWorldLogic::init()
{
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////
int AppWorldLogic::update()
{
	// Write here code to be called before updating each render frame: specify all graphics-related functions you want to be called every frame while your application executes.
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppWorldLogic::shutdown()
{
	return 1;
}
