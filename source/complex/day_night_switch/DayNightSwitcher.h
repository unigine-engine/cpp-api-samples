#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineLights.h>
#include "SunController.h"


class DayNightSwitcher : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(DayNightSwitcher, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the day and night switching.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	enum CONTROL_TYPE {
		Zenith = 0,
		Time = 1,
	};

	void setControlType(CONTROL_TYPE type);
	void setZenithThreshold(float value);
	float getZenithThreshold() { return sun_zenit_threshold; };
	void setControlMorningTime(Unigine::Math::ivec2 time_morning);
	void setControlEveningTime(Unigine::Math::ivec2 time_evening);
	int getControlMorningTime() { return time_morning.get().x * 60 + time_morning.get().y; };
	int getControlEveningTime() { return time_evening.get().x * 60 + time_evening.get().y; };

private:
	PROP_PARAM(Node, sun_node, "Sun node", "Sun node");

	PROP_PARAM(Switch, selected_control_type, 0, "Zenith,Time");
	PROP_PARAM(Float, sun_zenit_threshold, 85.0f, nullptr, nullptr, nullptr, "selected_control_type=0");
	PROP_PARAM(IVec2, time_morning, Unigine::Math::ivec2(7, 30), nullptr, nullptr, nullptr, "selected_control_type=1");
	PROP_PARAM(IVec2, time_evening, Unigine::Math::ivec2(19, 30), nullptr, nullptr, nullptr, "selected_control_type=1");

	PROP_PARAM(String, emission_material_parameter_name, "emission_scale");

	PROP_ARRAY(Material, materials_day_enabled);
	PROP_ARRAY(Material, materials_night_enabled);
	PROP_ARRAY(Node, nodes_day_enabled);
	PROP_ARRAY(Node, nodes_night_enabled);

	Unigine::HashMap<Unigine::UGUID, float> default_emission_scale;

	SunController* sun = nullptr;
	int is_day = -1;// is current sun rotation is concedered to be a day. 1:day, 0:night, -1:uninitialized
	CONTROL_TYPE switch_control_type = CONTROL_TYPE::Zenith;

	void init();
	void shutdown();
	void onTimeChange();
	void switchNodes(bool day);
};

