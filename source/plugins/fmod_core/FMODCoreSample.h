#pragma once

#include <UnigineComponentSystem.h>
#include <plugins/Unigine/FMOD/UnigineFMOD.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

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

	bool plugin_initialized = false;

	Unigine::ObjectMeshDynamicPtr car_sphere;

	Unigine::Plugins::FMOD::Sound *music_sound = nullptr;
	Unigine::Plugins::FMOD::Sound *music_sound_3D = nullptr;
	Unigine::Plugins::FMOD::Channel *music_channel = nullptr;
	Unigine::Plugins::FMOD::Channel *music_channel_3D = nullptr;

	SampleDescriptionWindow sample_description_window;
	Unigine::WidgetSliderPtr music_position_slider;
	Unigine::WidgetSliderPtr distortion_slider;
	Unigine::WidgetSliderPtr volume_slider;
};