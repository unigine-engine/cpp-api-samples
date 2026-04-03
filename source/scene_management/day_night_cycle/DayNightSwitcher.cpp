#pragma once
#include "DayNightSwitcher.h"
#include <UnigineConsole.h>


using namespace Unigine;
using namespace Math;

// Switches scene elements between day/night states based on sun position. Two modes:
// zenith angle threshold (checks sun's angle from vertical) or time-based (morning/evening
// bounds). Toggles node visibility and material emission parameters.

REGISTER_COMPONENT(DayNightSwitcher);

// SunController is obtained, event is subscribed, and default emission values are cached.
void DayNightSwitcher::init()
{
	sun = getComponent<SunController>(sun_node.get());
	if (!sun)
	{
		Log::error("DayNightSwitchSample::init can't find SunController component on the sun node!\n");
	}
	sun->getEventOnTimeChanged().connect(this, &DayNightSwitcher::onTimeChange);
	switch_control_type = static_cast<CONTROL_TYPE>(selected_control_type.get());


	for (int i = 0; i < materials_day_enabled.size(); i++)
	{
		MaterialPtr mat = materials_day_enabled.get(i).get();

		if (mat)
		{
			int param = mat->findParameter(emission_material_parameter_name.get());
			if (param == -1)
			{
				Log::error("DayNightSwitchSample::init materials_day_enabled got wrong material without emission!\n");
			}
			default_emission_scale.append(mat->getGUID(), mat->getParameterFloat(param));
		}
	}
	for (int i = 0; i < materials_night_enabled.size(); i++)
	{
		MaterialPtr mat = materials_night_enabled.get(i).get();

		if (mat)
		{
			int param = mat->findParameter(emission_material_parameter_name.get());
			if (param == -1)
			{
				Log::error("DayNightSwitchSample::init materials_night_enabled got wrong material without emission!\n");
			}
			default_emission_scale.append(mat->getGUID(), mat->getParameterFloat(param));
		}
	}
	onTimeChange();
}

// Event is unsubscribed and cached data is cleared.
void DayNightSwitcher::shutdown()
{
	default_emission_scale.clear();
	sun->getEventOnTimeChanged().disconnect(this, &DayNightSwitcher::onTimeChange);
	sun = nullptr;
}

// Day/night state is evaluated and nodes are switched if state changed.
void DayNightSwitcher::onTimeChange()
{
	switch (switch_control_type)
	{
	case 0: {// Zenith angle
		// Check if it is day after sun is rotated
		bool day = true;
		if (sun_node)
		{
			float current_angle = getAngle(vec3_up, sun_node->getWorldDirection(AXIS_Z));
			day = current_angle < sun_zenit_threshold;
		}

		// is_day initialized value is -1 so that nodes are always switched on first call
		if ((day ? 1 : 0) != is_day)
		{
			switchNodes(day);
			is_day = day;
		}
		break;
	}
	case 1: {// Time-based control
		int time = sun->getTime()/60;
		// Check if current time is within day bounds (morning to evening)
		bool day = time > (time_morning.get().x * 60 + time_morning.get().y)
				&& time < (time_evening.get().x * 60 + time_evening.get().y);
		// is_day initialized value is -1 so that nodes are always switched on first call
		if ((day ? 1 : 0) != is_day)
		{
			switchNodes(day);
			is_day = day;
		}
		break;
	}
	default:
		break;
	}

}

// Node visibility and material emission are toggled based on day/night state.
void DayNightSwitcher::switchNodes(bool day)
{
	// Materials
	for (size_t i = 0; i < materials_day_enabled.size(); i++)
	{
		MaterialPtr mat = materials_day_enabled.get(i).get();
		if (mat)
		{
			mat->setParameterFloat(emission_material_parameter_name.get(), day ? default_emission_scale[mat->getGUID()] : 0);
		}
		else
		{
			Log::warning("DayNightSwitcher::on_time_changed: materials_day_enabled  wrong day material %d\n", i);
		}

	}

	for (size_t i = 0; i < materials_night_enabled.size(); i++)
	{
		MaterialPtr mat = materials_night_enabled.get(i).get();
		if (mat)
		{
			mat->setParameterFloat(emission_material_parameter_name.get(), !day ? default_emission_scale[mat->getGUID()] : 0);
		}
		else
		{
			Log::warning("DayNightSwitcher::on_time_changed: materials_night_enabled wrong day material %d\n", i);
		}

	}

	// Nodes
	for (size_t i = 0; i < nodes_day_enabled.size(); i++)
	{
		NodePtr node = nodes_day_enabled.get(i).get();
		if (node)
		{
			node->setEnabled(day);
		}
		else
		{
			Log::warning("DayNightSwitcher::on_time_changed: wrong node %d\n", i);
		}
	}

	for (size_t i = 0; i < nodes_night_enabled.size(); i++)
	{
		NodePtr node = nodes_night_enabled.get(i).get();
		if (node)
		{
			node->setEnabled(!day);
		}
		else
		{
			Log::warning("DayNightSwitcher::on_time_changed: wrong node %d\n", i);
		}
	}
}

// Control type is changed and day/night state is re-evaluated immediately.
void DayNightSwitcher::setControlType(CONTROL_TYPE type)
{
	switch_control_type = type;
	// Apply new threshold instantly
	onTimeChange();
}

// Zenith threshold is clamped and applied, day/night state is re-evaluated.
void DayNightSwitcher::setZenithThreshold(float value)
{
	value = clamp(value, 0.0f, 180.0f);
	sun_zenit_threshold = value;
	// Apply new threshold instantly
	onTimeChange();
}

// Morning time bound is set and day/night state is re-evaluated.
void DayNightSwitcher::setControlMorningTime(Unigine::Math::ivec2 time_morning)
{
	this->time_morning = time_morning;
	// Apply change instantly
	onTimeChange();
}
// Evening time bound is set and day/night state is re-evaluated.
void DayNightSwitcher::setControlEveningTime(Unigine::Math::ivec2 time_evening)
{
	this->time_evening = time_evening;
	// Apply change instantly
	onTimeChange();
}