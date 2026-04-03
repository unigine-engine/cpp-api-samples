// Demonstrates events with multiple arguments. Keyboard triggers fire events with
// varying argument counts: T/Y/U for single-axis rotation (1 arg), I for combined
// XYZ rotation (4 args including sender pointer for callback context).

#include "EventsAdvancedSample.h"
#include <UnigineConsole.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(EventsAdvancedSample);

using namespace Unigine;

// Console is enabled for onscreen logging.
void EventsAdvancedSample::init()
{
	Console::setOnscreen(true);
}

// Keyboard input is checked and corresponding rotation events are fired.
void EventsAdvancedSample::update()
{
	// Skip input processing if console is active
	if (Console::isActive())
		return;

	// Trigger rotate X event with one argument when T is pressed
	if (Input::isKeyPressed(Input::KEY_T))
	{
		Log::message("Run rotate X with 1 arg\n");
		rotate_x_event.run(rotation_speed.get().x);
	}

	// Trigger rotate Y event with one argument when Y is pressed
	if (Input::isKeyPressed(Input::KEY_Y))
	{
		Log::message("Run rotate Y with 1 arg\n");
		rotate_y_event.run(rotation_speed.get().y);
	}

	// Trigger rotate Z event with one argument when U is pressed
	if (Input::isKeyPressed(Input::KEY_U))
	{
		Log::message("Run rotate Z with 1 arg\n");
		rotate_z_event.run(rotation_speed.get().z);
	}

	// Trigger rotate XYZ event with 4 arguments when I is pressed
	if (Input::isKeyPressed(Input::KEY_I))
	{
		Log::message("Run rotate XYZ with 4 args\n");
		rotate_event.run(rotation_speed.get().x, rotation_speed.get().y, rotation_speed.get().z, this);
	}
}

// Console is disabled on shutdown.
void EventsAdvancedSample::shutdown()
{
	Console::setOnscreen(false);
}
