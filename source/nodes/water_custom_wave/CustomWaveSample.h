#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class CustomWaveSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CustomWaveSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, wind_direction_angle, 90.0f);
	PROP_PARAM(Int, num_octaves, 5);
	PROP_PARAM(Int, num_waves_per_octave, 4);
	PROP_PARAM(Float, steepness_scale, 1.0f);

private:
	void init();
	void shutdown();

	void refresh_wave_param();

	void init_gui();
	SampleDescriptionWindow sample_description_window;
	Unigine::Math::Random random;
	Unigine::ObjectWaterGlobalPtr water;

	float len_mean = 0.5f;
	float len_spread = 0.5f;
	float amp_mean = 0.5f;
	float amp_spread = 0.5f;
	float phase_mean = 0.0f;
	float phase_spread = Unigine::Math::Consts::PI;
};
