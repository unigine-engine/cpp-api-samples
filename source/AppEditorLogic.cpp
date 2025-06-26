#include "AppEditorLogic.h"

// Editor logic, it takes effect only when the UnigineEditor is loaded.
// These methods are called right after corresponding editor script's (UnigineScript) methods.

AppEditorLogic::AppEditorLogic()
{
}

AppEditorLogic::~AppEditorLogic()
{
}

int AppEditorLogic::init()
{
	// Write here code to be called on editor initialization.
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// start of the main loop
////////////////////////////////////////////////////////////////////////////////
int AppEditorLogic::update()
{
	// Write here code to be called before updating each render frame when editor is loaded.
	return 1;
}

int AppEditorLogic::render()
{
	// Write here code to be called before rendering each render frame when editor is loaded.
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// end of the main loop
////////////////////////////////////////////////////////////////////////////////

int AppEditorLogic::shutdown()
{
	// Write here code to be called on editor shutdown.
	return 1;
}

int AppEditorLogic::destroy()
{
	// Write here code to be called when the video mode is changed or the application is restarted (i.e. video_restart is called). It is used to reinitialize the graphics context.
	return 1;
}

int AppEditorLogic::worldInit()
{
	// Write here code to be called on world initialization when editor is loaded.
	return 1;
}

int AppEditorLogic::worldShutdown()
{
	// Write here code to be called on world shutdown when editor is loaded.
	return 1;
}

int AppEditorLogic::worldSave()
{
	// Write here code to be called on world save when editor is loaded.
	return 1;
}
