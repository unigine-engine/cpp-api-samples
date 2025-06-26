#include "EventsAdvancedSample.h"
#include <UnigineConsole.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(EventsAdvancedSample);

using namespace Unigine;

void EventsAdvancedSample::init()
{
	Console::setOnscreen(true);
}

void EventsAdvancedSample::update()
{
	if (Console::isActive())
		return;

	if (Input::isKeyPressed(Input::KEY_T))
	{
		Log::message("Run rotate X with 1 arg\n");
		rotate_x_event.run(rotation_speed.get().x);
	}
	if (Input::isKeyPressed(Input::KEY_Y))
	{
		Log::message("Run rotate Y with 1 arg\n");
		rotate_y_event.run(rotation_speed.get().y);
	}
	if (Input::isKeyPressed(Input::KEY_U))
	{
		Log::message("Run rotate Z with 1 arg\n");
		rotate_z_event.run(rotation_speed.get().z);
	}
	if (Input::isKeyPressed(Input::KEY_I))
	{
		Log::message("Run rotate XYZ with 4 args\n");
		rotate_event.run(rotation_speed.get().x, rotation_speed.get().y, rotation_speed.get().z, this);
	}
}

void EventsAdvancedSample::shutdown()
{
	Console::setOnscreen(false);
}
