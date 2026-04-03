#pragma once
#include "SunController.h"

using namespace Unigine;
using namespace Math;

// Controls sun rotation to simulate day/night cycle. Time (in seconds) is mapped to
// rotation angle: 0 sec = -180°, max_time = +180°. Fires time_changed_event for UI sync.
REGISTER_COMPONENT(SunController);

// Initial sun rotation is stored for calculating time-based offsets.
void SunController::init()
{
	sun_init_tilt = node->getWorldRotation();
}

// Time is advanced and sun position is updated in continuous mode.
void SunController::update()
{
	if (is_continuous)
	{
		current_time += Game::getIFps() * timescale;
		if (current_time > max_time_sec)
		{
			// Wrap around without losing extra delta time
			current_time -= max_time_sec;
		}
		refreshSunPosition();
		// Notify listeners about time change
		time_changed_event.run(current_time);
	}
}

// Continuous rotation mode is enabled or disabled.
void SunController::setContinuousEnabled(bool value)
{
	is_continuous = value;
}

// Time multiplier is set for continuous rotation speed.
void SunController::setTimescale(float timescale)
{
	this->timescale = timescale;
}

// Sun rotation is calculated from current time and applied to the node.
void SunController::refreshSunPosition()
{
	// Converting time to rotation
	float time = fmodf(current_time, max_time_sec);
	float k = Math::inverseLerp(0.0f, max_time_sec, time);
	// +180 deg offset so that 12:00 corresponds to noon
	float angle = Math::lerp(-180.0f, 180.0f, k);
	node->setWorldRotation(sun_init_tilt * quat(angle, 0.0f, 0.0f), true);
}

// Time is set manually and sun position is updated immediately.
void SunController::setTime(int t)
{
	current_time = t;
	refreshSunPosition();

	time_changed_event.run(current_time);
}
