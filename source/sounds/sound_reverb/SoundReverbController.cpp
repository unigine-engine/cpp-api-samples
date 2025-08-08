#include "SoundReverbController.h"

#include <UnigineVisualizer.h>

REGISTER_COMPONENT(SoundReverbController)

using namespace Unigine;
using namespace Math;


void SoundReverbController::init()
{
	sound_source = checked_ptr_cast<SoundSource>(sound_node.get());
	if (!sound_source)
		Log::error("SoundReverbController::init(): Sound Source node must be SoundeSource type\n");

	sound_reverb = SoundReverb::create(vec3(20.0f, 20.0f, 20.0f));
	sound_reverb->setWorldTransform(Mat4_identity);
	sound_reverb->setThreshold(vec3(10.0f, 10.0f, 10.0f));

	update_reverb_settings();

	// create settings UI
	sample_description_window.createWindow();

	sample_description_window.addFloatParameter("Gain:", "Gain", reverb_power, 0.0f, 1.0f,
		[this](float val) {
			reverb_power = val;
			update_reverb_settings();
		});

	Visualizer::setEnabled(true);
}

void SoundReverbController::update()
{
	if (!sound_reverb || !sound_source)
		return;

	sound_reverb->renderVisualizer();
	sound_source->renderVisualizer();
}

void SoundReverbController::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

void SoundReverbController::update_reverb_settings()
{
	sound_reverb->setDensity(clamp(1.0f - reverb_power, 0.0f, 1.0f));
	sound_reverb->setDiffusion(clamp(1.0f - reverb_power, 0.0f, 1.0f));
	sound_reverb->setDecayTime(clamp(0.1f + 19.9f * reverb_power, 0.1f, 20.0f));
	sound_reverb->setReflectionGain(clamp(3.16f * reverb_power, 0.0f, 2.16f));
	sound_reverb->setLateReverbGain(clamp(10.0f * reverb_power, 0.0f, 10.0f));
}
