// FMOD Core API integration sample demonstrating low-level audio control.
// Shows 2D/3D sound loading, DSP effects (distortion), timeline control, and volume.
// A visible sphere represents the 3D sound source position in the scene.

#pragma once

#include <UnigineComponentSystem.h>
#include <plugins/Unigine/FMOD/UnigineFMOD.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates FMOD Core API for direct sound and channel control.
class FMODCoreSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(FMODCoreSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void init_description_window();

	void distortion_changed();
	void volume_changed();
	void plus_ms();
	void minus_ms();
	void play_music();
	void stop_music();
	void toggle_pause_music();
	void play_music_3D();
	void stop_music_3D();
	void toggle_pause_music_3D();

	// Tracks whether FMOD plugin loaded successfully
	bool plugin_initialized = false;

	// Visual representation of 3D sound position
	Unigine::ObjectMeshDynamicPtr car_sphere;

	// FMOD sound objects for 2D and 3D playback
	Unigine::Plugins::FMOD::Sound *music_sound = nullptr;
	Unigine::Plugins::FMOD::Sound *music_sound_3D = nullptr;
	// Active playback channels for 2D and 3D sounds
	Unigine::Plugins::FMOD::Channel *music_channel = nullptr;
	Unigine::Plugins::FMOD::Channel *music_channel_3D = nullptr;

	SampleDescriptionWindow sample_description_window;
	// UI controls for playback position, distortion effect, and volume
	Unigine::WidgetSliderPtr music_position_slider;
	Unigine::WidgetSliderPtr distortion_slider;
	Unigine::WidgetSliderPtr volume_slider;
};