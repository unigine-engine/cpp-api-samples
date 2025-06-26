#pragma once
#include "SunController.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SunController);

void SunController::init()
{
	sun_init_tilt = node->getWorldRotation();
}

void SunController::update()
{
	if (is_continuous)
	{
		current_time += Game::getIFps() * timescale;
		if (current_time > max_time_sec)
		{
			current_time -= max_time_sec;//so we wont loose extra delta time 
		}
		refreshSunPosition();;
		time_changed_event.run(static_cast<int>(current_time));//displaying only integer part
	}
}

void SunController::setContinuousEnabled(bool value)
{
	is_continuous = value;
}

void SunController::setTimescale(float timescale)
{
	this->timescale = timescale;
}

void SunController::refreshSunPosition()
{
	//converting time to rotation
	float time = fmodf(current_time, max_time_sec);
	float k = Math::inverseLerp(0.0f, max_time_sec, time);
	float angle = Math::lerp(-180.0f, 180.0f, k);// +180 deg  so that 12:00 corresponds to noon
	node->setWorldRotation(sun_init_tilt * quat(angle, 0.0f, 0.0f), true);
}

void SunController::setTime(int t)
{
	current_time = t;
	refreshSunPosition();

	time_changed_event.run(static_cast<int>(current_time));
}
