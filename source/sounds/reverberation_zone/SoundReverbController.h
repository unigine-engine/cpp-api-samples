// Demonstrates SoundReverb zone for environmental audio effects.
// Creates a reverb volume that affects sound sources within its bounds.
// Reverb parameters are controlled via a single power slider for easy adjustment.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Creates and controls a reverb zone affecting nearby sound sources.
class SoundReverbController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundReverbController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to the sound source node to visualize
	PROP_PARAM(Node, sound_node, "Sound Source");

private:
	void init();
	void update();
	void shutdown();

	void update_reverb_settings();

private:
	// Sound source for visualization
	Unigine::SoundSourcePtr sound_source;

	// Reverb zone affecting sounds within its bounds
	Unigine::SoundReverbPtr sound_reverb;
	// Master control value for all reverb parameters (0-1)
	float reverb_power = 0.5f;

	SampleDescriptionWindow sample_description_window;
};
