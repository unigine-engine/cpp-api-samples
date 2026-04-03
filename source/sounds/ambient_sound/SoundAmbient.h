// Demonstrates ambient (non-positional) sound playback using AmbientSource.
// Supports play/stop, loop, gain, pitch controls, and stream/buffer mode switching.
// Ideal for background music and UI sounds that don't need 3D spatialization.

#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

// Plays non-positional audio with runtime stream mode switching.
class SoundAmbient : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundAmbient, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Path to the audio file to play
	PROP_PARAM(File, sound_file);

private:
	void init();
	void shutdown();

	void change_souce_type();

private:
	// Audio source for non-positional playback
	Unigine::AmbientSourcePtr ambient_source;
	// Current playback mode (true = streaming, false = buffered)
	bool is_stream = false;

	SampleDescriptionWindow sample_description_window;
};
