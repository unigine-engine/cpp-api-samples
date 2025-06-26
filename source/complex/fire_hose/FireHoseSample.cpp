#include "FireHoseSample.h"
#include "ExtinguishingPuddles.h"
#include "UnigineGame.h"

REGISTER_COMPONENT(FireHoseSample);

using namespace Unigine;
using namespace Math;

void FireHoseSample::init()
{
	mouse_grab_state_at_init = Input::isMouseGrab();
	mouse_handle_at_init = Input::getMouseHandle();
	Input::setMouseGrab(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	description_window.createWindow();

	WidgetCheckBoxPtr rotate_check = WidgetCheckBox::create();
	rotate_check->setText(" Rotate");
	rotate_check->setChecked(emitter_rotate);
	rotate_check->getEventChanged().connect(*this, [this, rotate_check]() {
		if (emitter)
		{
			emitter_rotate = rotate_check->isChecked();
			emitter->setRotation(quat(vec3_up, 0));
			if (rotate_check->isChecked())
				time_start = Game::getTime();
		}
		});

	description_window.getParameterGroupBox()->addChild(rotate_check, Gui::ALIGN_LEFT);

	description_window.addFloatParameter("Angle", "", 0.0f, -50.0f, 50.0f, [this](float value) {
		if (emitter && !emitter_rotate)
		{
			emitter->setRotation(quat(vec3_up, -value ));
		}
		});
	
}

void FireHoseSample::update()
{
	if (emitter && emitter_rotate)
	{
		float delta = 11.0f * Game::getIFps();
		emitter->rotate(quat(emitter->getWorldDirection(AXIS_Z), delta * ((cosf((Game::getTime() - time_start) / 3)) > 0.0f ? 1 : -1)));
	}
}

void FireHoseSample::shutdown()
{
	Input::setMouseGrab(mouse_grab_state_at_init);
	Input::setMouseHandle(mouse_handle_at_init);

	description_window.shutdown();
}
