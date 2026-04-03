// Demonstrates manual wave generation for ObjectWaterGlobal. In manual mode,
// individual waves are defined with custom parameters (length, amplitude, steepness,
// direction, phase) instead of using the automatic Beaufort-based wave spectrum.

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

	// Wind direction affects wave propagation angle
	PROP_PARAM(Float, wind_direction_angle, 90.0f);
	// Octaves create waves at different frequency scales (like audio octaves)
	PROP_PARAM(Int, num_octaves, 5);
	// Multiple waves per octave add variation within each frequency band
	PROP_PARAM(Int, num_waves_per_octave, 4);
	// Steepness controls wave sharpness (higher = more peaked waves)
	PROP_PARAM(Float, steepness_scale, 1.0f);

private:
	void init();
	void shutdown();

	void refresh_wave_param();

	void init_gui();
	SampleDescriptionWindow sample_description_window;
	// Seeded random generator for reproducible wave spectra
	Unigine::Math::Random random;
	Unigine::ObjectWaterGlobalPtr water;

	// Mean and spread define parameter ranges for randomization
	float len_mean = 0.5f;
	float len_spread = 0.5f;
	float amp_mean = 0.5f;
	float amp_spread = 0.5f;
	// Phase offset in radians (0 to 2*PI range)
	float phase_mean = 0.0f;
	float phase_spread = Unigine::Math::Consts::PI;
};
