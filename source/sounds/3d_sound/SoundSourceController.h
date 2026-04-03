// Demonstrates 3D positional sound using SoundSource node.
// Audio is spatialized based on listener distance with configurable falloff.
// Supports streaming mode, loop, gain, pitch controls, and debug visualization.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Creates and controls a 3D positional sound source in the scene.
class SoundSourceController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundSourceController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Path to the audio file to play
	PROP_PARAM(File, sound_file);

private:
	void init();
	void update();
	void shutdown();

private:
	// 3D positional sound source node
	Unigine::SoundSourcePtr sound_source;

	SampleDescriptionWindow sample_description_window;
};
