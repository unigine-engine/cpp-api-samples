// Demonstrates SoundReverb zone for environmental audio effects. Creates a reverb
// volume that affects sound sources within its bounds. Reverb parameters (density,
// diffusion, decay time, reflection gain) are adjusted via a single power slider.

#include "SoundReverbController.h"

#include <UnigineVisualizer.h>

REGISTER_COMPONENT(SoundReverbController)

using namespace Unigine;
using namespace Math;


// Reverb zone is created and UI slider is set up.
void SoundReverbController::init()
{
	sound_source = checked_ptr_cast<SoundSource>(sound_node.get());
	if (!sound_source)
		Log::error("SoundReverbController::init(): Sound Source node must be SoundeSource type\n");

	// Create reverb zone with outer and inner (threshold) bounds
	sound_reverb = SoundReverb::create(vec3(20.0f, 20.0f, 20.0f));
	sound_reverb->setWorldTransform(Mat4_identity);
	sound_reverb->setThreshold(vec3(10.0f, 10.0f, 10.0f));

	update_reverb_settings();

	// Create settings UI with power slider
	sample_description_window.createWindow();

	sample_description_window.addFloatParameter("Gain:", "Gain", reverb_power, 0.0f, 1.0f,
		[this](float val) {
			reverb_power = val;
			update_reverb_settings();
		});

	Visualizer::setEnabled(true);
}

// Reverb zone and sound source bounds are visualized.
void SoundReverbController::update()
{
	if (!sound_reverb || !sound_source)
		return;

	sound_reverb->renderVisualizer();
	sound_source->renderVisualizer();
}

// Visualizer is disabled and UI is cleaned up.
void SoundReverbController::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

// All reverb parameters are derived from single power value.
void SoundReverbController::update_reverb_settings()
{
	// Higher power = less density/diffusion, longer decay, stronger reflections
	sound_reverb->setDensity(clamp(1.0f - reverb_power, 0.0f, 1.0f));
	sound_reverb->setDiffusion(clamp(1.0f - reverb_power, 0.0f, 1.0f));
	sound_reverb->setDecayTime(clamp(0.1f + 19.9f * reverb_power, 0.1f, 20.0f));
	sound_reverb->setReflectionGain(clamp(3.16f * reverb_power, 0.0f, 2.16f));
	sound_reverb->setLateReverbGain(clamp(10.0f * reverb_power, 0.0f, 10.0f));
}
