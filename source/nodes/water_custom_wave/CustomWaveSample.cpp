#include "CustomWaveSample.h"

REGISTER_COMPONENT(CustomWaveSample);

using namespace Unigine;
using namespace Math;

void CustomWaveSample::init()
{
	// change preset to custom (4)
	Render::setWaterGeometryPreset(4);
	Render::setWaterGeometryPolygonSize(0.01f);
	Render::setWaterGeometryProgression(1);
	Render::setWaterGeometrySubpixelReduction(6);

	water = ObjectWaterGlobal::create();
	// set manual mode for create your own custom waves
	water->setWavesMode(ObjectWaterGlobal::WAVES_MODE_MANUAL);


	init_gui();
	refresh_wave_param();
}

void CustomWaveSample::shutdown()
{
	water.deleteLater();
	sample_description_window.shutdown();
}

void CustomWaveSample::init_gui()
{
	sample_description_window.createWindow();

	sample_description_window.addIntParameter("Number of Octaves", "", num_octaves, 1, 5, [this](int v)
	{
		num_octaves = v;
		refresh_wave_param();
	});

	sample_description_window.addIntParameter("Number of Waves Per Octave", "", num_waves_per_octave, 1, 10, [this](int v)
	{
		num_waves_per_octave = v;
		refresh_wave_param();
	});

	sample_description_window.addFloatParameter("Stepness Scale", "", steepness_scale, 0.01f, 1.0f, [this](float v)
	{
		steepness_scale = v;
		refresh_wave_param();
	});

	sample_description_window.addFloatParameter("Length Mean", "", len_mean, 0.0f, 2.0f, [this](float v)
	{
		len_mean = v;
		refresh_wave_param();
	});

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
	const float len_min = Unigine::Math::clamp(len_mean - len_spread, 0.0f, 100.0f);
	const float len_max = Unigine::Math::clamp(len_mean + len_spread, 0.0f, 100.0f);
	const float amplitude_min = Unigine::Math::clamp(amp_mean - amp_spread, 0.0f, 100.0f);
	const float amplitude_max = Unigine::Math::clamp(amp_mean + amp_spread, 0.0f, 100.0f);
	const float phase_min = Unigine::Math::clamp(phase_mean - phase_spread, -Consts::PI2, Consts::PI2);
	const float phase_max = Unigine::Math::clamp(phase_mean + phase_spread, -Consts::PI2, Consts::PI2);

	// clear waves
	while (water->getNumWaves())
		water->removeWave(water->getNumWaves() - 1);

	// set wind direction
	water->setWindDirectionAngle(wind_direction_angle);

	float num_waves = (float)(num_octaves * num_waves_per_octave);
	float inv_num_waves = rcp(num_waves);
	for (int i = 0; i < num_octaves; i++)
	{
		for (int j = 0; j < num_waves_per_octave; j++)
		{
			// Randomize wave parameters
			// set seed as wave index to get the same spectre on each program run
			random.setSeed(num_waves_per_octave * i + j);

			// get wave length as a power of two
			float wave_length = powf(2.0f, float(i - 1) + random.getFloat(len_min, len_max));

			// 0.25f is maximum height of wave
			float amplitude = (float(i) + random.getFloat(amplitude_min, amplitude_max)) * inv_num_waves * 0.25f;

			// get the direction angle in a range 0.0f.. 140.0f and take into account the wind direction
			float direction_angle = random.getFloat(-1.0f, 1.0f) * 140.0f + wind_direction_angle;

			float phase_offset = random.getFloat(phase_min, phase_max);

			// in manual mode we should calculate steepness ourseves
			float steepness = steepness_scale / max(amplitude * (Consts::PI / wave_length) * num_waves, Consts::EPS);

			// add wave to water object
			water->addWave(wave_length, amplitude, steepness, direction_angle, phase_offset);
		}
	}
}
