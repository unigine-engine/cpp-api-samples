// Demonstrates manual wave generation for ObjectWaterGlobal. In manual mode,
// individual waves are defined with custom parameters (length, amplitude, steepness,
// direction, phase) instead of using the automatic Beaufort-based wave spectrum.

#include "CustomWaveSample.h"

REGISTER_COMPONENT(CustomWaveSample);

using namespace Unigine;
using namespace Math;

void CustomWaveSample::init()
{
	// Switch to custom water geometry preset (index 4)
	// This provides fine control over tessellation and LOD settings
	Render::setWaterGeometryPreset(4);
	// Small polygon size = higher tessellation for detailed wave shapes
	Render::setWaterGeometryPolygonSize(0.01f);
	// Linear progression (1) for uniform tessellation across distance
	Render::setWaterGeometryProgression(1);
	// Subpixel reduction controls LOD aggressiveness for small polygons
	Render::setWaterGeometrySubpixelReduction(6);

	water = ObjectWaterGlobal::create();
	// Manual mode disables automatic Beaufort-based wave generation
	// Waves must be added explicitly via addWave()
	water->setWavesMode(ObjectWaterGlobal::WAVES_MODE_MANUAL);


	init_gui();
	refresh_wave_param();
}

void CustomWaveSample::shutdown()
{
	// deleteLater() ensures safe removal at end of frame
	water.deleteLater();
	sample_description_window.shutdown();
}

void CustomWaveSample::init_gui()
{
	sample_description_window.createWindow();

	// Octaves represent frequency bands (each octave doubles wavelength)
	sample_description_window.addIntParameter("Number of Octaves", "", num_octaves, 1, 5, [this](int v)
	{
		num_octaves = v;
		refresh_wave_param();
	});

	// More waves per octave creates a richer, more complex water surface
	sample_description_window.addIntParameter("Number of Waves Per Octave", "", num_waves_per_octave, 1, 10, [this](int v)
	{
		num_waves_per_octave = v;
		refresh_wave_param();
	});

	// Steepness affects wave peakedness (Gerstner wave parameter)
	sample_description_window.addFloatParameter("Stepness Scale", "", steepness_scale, 0.01f, 1.0f, [this](float v)
	{
		steepness_scale = v;
		refresh_wave_param();
	});

	// Length mean: center value for wavelength randomization
	sample_description_window.addFloatParameter("Length Mean", "", len_mean, 0.0f, 2.0f, [this](float v)
	{
		len_mean = v;
		refresh_wave_param();
	});

	// Length spread: variation range around mean (+/- this value)
	sample_description_window.addFloatParameter("Length Spread", "", len_spread, 0.0f, 2.0f, [this](float v)
	{
		len_spread = v;
		refresh_wave_param();
	});

	sample_description_window.addFloatParameter("Amplitude Mean", "", amp_mean, 0.0f, 25.0f, [this](float v)
	{
		amp_mean = v;
		refresh_wave_param();
	});

	sample_description_window.addFloatParameter("Amplitude Spread", "", amp_spread, 0.0f, 25.0f, [this](float v)
	{
		amp_spread = v;
		refresh_wave_param();
	});

	// Phase offset shifts wave position along its direction
	sample_description_window.addFloatParameter("Phase Offset Mean", "", phase_mean, 0.0f, Consts::PI2, [this](float v)
	{
		phase_mean = v;
		refresh_wave_param();
	});

	sample_description_window.addFloatParameter("Phase Offset Spread", "", phase_spread, 0.0f, Consts::PI2, [this](float v)
	{
		phase_spread = v;
		refresh_wave_param();
	});
}

void CustomWaveSample::refresh_wave_param()
{
	// Calculate min/max ranges from mean +/- spread, clamped to valid bounds
	const float len_min = Unigine::Math::clamp(len_mean - len_spread, 0.0f, 100.0f);
	const float len_max = Unigine::Math::clamp(len_mean + len_spread, 0.0f, 100.0f);
	const float amplitude_min = Unigine::Math::clamp(amp_mean - amp_spread, 0.0f, 100.0f);
	const float amplitude_max = Unigine::Math::clamp(amp_mean + amp_spread, 0.0f, 100.0f);
	// Phase can be negative for backwards offset
	const float phase_min = Unigine::Math::clamp(phase_mean - phase_spread, -Consts::PI2, Consts::PI2);
	const float phase_max = Unigine::Math::clamp(phase_mean + phase_spread, -Consts::PI2, Consts::PI2);

	// Remove all existing waves before regenerating
	// Removal is done from end to avoid index shifting issues
	while (water->getNumWaves())
		water->removeWave(water->getNumWaves() - 1);

	// Wind direction is the primary propagation angle for all waves
	water->setWindDirectionAngle(wind_direction_angle);

	float num_waves = (float)(num_octaves * num_waves_per_octave);
	// Reciprocal for normalizing amplitude across wave count
	float inv_num_waves = rcp(num_waves);
	for (int i = 0; i < num_octaves; i++)
	{
		for (int j = 0; j < num_waves_per_octave; j++)
		{
			// Set deterministic seed based on wave index for reproducible results
			// Same parameters always produce identical wave spectra
			random.setSeed(num_waves_per_octave * i + j);

			// Wavelength doubles with each octave (power of 2 scaling)
			// Higher octaves = longer, slower waves
			float wave_length = powf(2.0f, float(i - 1) + random.getFloat(len_min, len_max));

			// Amplitude normalized by wave count to prevent excessive height
			// 0.25f is maximum relative height per wave
			float amplitude = (float(i) + random.getFloat(amplitude_min, amplitude_max)) * inv_num_waves * 0.25f;

			// Direction varies +/- 70 degrees from wind direction
			// Creates realistic wave spread pattern
			float direction_angle = random.getFloat(-1.0f, 1.0f) * 140.0f + wind_direction_angle;

			float phase_offset = random.getFloat(phase_min, phase_max);

			// Steepness formula prevents waves from looping over themselves
			// Based on Gerstner wave physics: steepness = Q / (amplitude * k * n)
			// where k = 2*PI/wavelength, n = number of waves
			float steepness = steepness_scale / max(amplitude * (Consts::PI / wave_length) * num_waves, Consts::EPS);

			// Add the configured wave to the water surface
			water->addWave(wave_length, amplitude, steepness, direction_angle, phase_offset);
		}
	}
}
