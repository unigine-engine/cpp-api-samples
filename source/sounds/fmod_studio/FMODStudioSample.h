// FMOD Studio API integration sample demonstrating event-based audio.
// Shows parameter control (ambience, engine RPM), VCA volume groups, and Doppler effect.
// A moving sphere demonstrates velocity-based pitch shifting for realistic motion audio.

#pragma once

#include <UnigineComponentSystem.h>
#include <plugins/Unigine/FMOD/UnigineFMOD.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates FMOD Studio events, parameters, VCA, and Doppler simulation.
class FMODStudioSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(FMODStudioSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void init_description_window();

	void env_vca_slider_changed();
	void engine_slider_changed();
	void wind_forest_slider_changed();
	void rain_forest_slider_changed();
	void cover_forest_slider_changed();
	void doppler_rpm_slider_changed();
	void doppler_velocity_slider_changed();

	// Tracks whether FMOD plugin loaded successfully
	bool plugin_initialized = false;

	// Timer for Doppler object position reset
	float timer = 0.0f;

	// Static sphere representing car sound position
	Unigine::ObjectMeshDynamicPtr car_sphere;
	// Moving sphere for Doppler effect demonstration
	Unigine::ObjectMeshDynamicPtr doppler_sphere;

	// FMOD Studio event instances for different sounds
	Unigine::Plugins::FMOD::EventInstance *engine_event = nullptr;
	Unigine::Plugins::FMOD::EventInstance *doppler_engine_event = nullptr;
	Unigine::Plugins::FMOD::EventInstance *forest_event = nullptr;
	// VCA for controlling environment sound group volume
	Unigine::Plugins::FMOD::VCA *env_vca = nullptr;

	// Current velocity of Doppler sphere for pitch calculation
	Unigine::Math::Vec3 velocity;
	// Starting position for Doppler sphere reset
	Unigine::Math::Vec3 start_point = Unigine::Math::Vec3(-5, 80, 0);

	SampleDescriptionWindow sample_description_window;
	// UI controls for various audio parameters
	Unigine::WidgetSliderPtr engine_slider;
	Unigine::WidgetSliderPtr wind_forest_slider;
	Unigine::WidgetSliderPtr rain_forest_slider;
	Unigine::WidgetSliderPtr cover_forest_slider;
	Unigine::WidgetSliderPtr env_vca_slider;
	Unigine::WidgetSliderPtr doppler_rpm_slider;
	Unigine::WidgetSliderPtr doppler_velocity_slider;
	Unigine::WidgetCheckBoxPtr show_doppler_box_check_box;
};