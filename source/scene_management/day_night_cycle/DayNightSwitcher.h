#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineLights.h>
#include "SunController.h"

// Switches scene elements between day and night states.
// Two control modes: zenith angle (sun's position relative to horizon)
// or explicit time bounds (morning/evening hours).
// Toggles node visibility and material emission parameters on transitions.
class DayNightSwitcher : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DayNightSwitcher, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the day and night switching.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Day/night detection mode
	enum CONTROL_TYPE {
		Zenith = 0, // Use sun angle from vertical
		Time = 1,   // Use time of day bounds
	};

	// Control type and threshold setters
	void setControlType(CONTROL_TYPE type);
	void setZenithThreshold(float value);
	float getZenithThreshold() { return sun_zenit_threshold; };

	// Time-based control bounds (hours:minutes as ivec2)
	void setControlMorningTime(Unigine::Math::ivec2 time_morning);
	void setControlEveningTime(Unigine::Math::ivec2 time_evening);
	int getControlMorningTime() { return time_morning.get().x * 60 + time_morning.get().y; };
	int getControlEveningTime() { return time_evening.get().x * 60 + time_evening.get().y; };

private:
	// Parameters
	PROP_PARAM(Node, sun_node, "Sun node", "Sun node"); // Reference to sun with SunController

	PROP_PARAM(Switch, selected_control_type, 0, "Zenith,Time"); // Detection mode selector
	PROP_PARAM(Float, sun_zenit_threshold, 85.0f, nullptr, nullptr, nullptr, "selected_control_type=0"); // Angle threshold for zenith mode
	PROP_PARAM(IVec2, time_morning, Unigine::Math::ivec2(7, 30), nullptr, nullptr, nullptr, "selected_control_type=1"); // Day start time
	PROP_PARAM(IVec2, time_evening, Unigine::Math::ivec2(19, 30), nullptr, nullptr, nullptr, "selected_control_type=1"); // Day end time

	PROP_PARAM(String, emission_material_parameter_name, "emission_scale"); // Material parameter to toggle

	// Arrays of objects to switch between day/night states
	PROP_ARRAY(Material, materials_day_enabled);   // Materials with emission enabled during day
	PROP_ARRAY(Material, materials_night_enabled); // Materials with emission enabled at night
	PROP_ARRAY(Node, nodes_day_enabled);           // Nodes visible during day
	PROP_ARRAY(Node, nodes_night_enabled);         // Nodes visible at night

	Unigine::HashMap<Unigine::UGUID, float> default_emission_scale;

	SunController* sun = nullptr;
	int is_day = -1; // Current state: 1=day, 0=night, -1=uninitialized
	CONTROL_TYPE switch_control_type = CONTROL_TYPE::Zenith;

	void init();
	void shutdown();
	void onTimeChange();
	void switchNodes(bool day);
};

