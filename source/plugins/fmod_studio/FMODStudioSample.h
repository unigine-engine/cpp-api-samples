#pragma once

#include <UnigineComponentSystem.h>
#include <plugins/Unigine/FMOD/UnigineFMOD.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

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

	bool plugin_initialized = false;

	float timer = 0.0f;

	Unigine::ObjectMeshDynamicPtr car_sphere;
	Unigine::ObjectMeshDynamicPtr doppler_sphere;

	Unigine::Plugins::FMOD::EventInstance *engine_event = nullptr;
	Unigine::Plugins::FMOD::EventInstance *doppler_engine_event = nullptr;
	Unigine::Plugins::FMOD::EventInstance *forest_event = nullptr;
	Unigine::Plugins::FMOD::VCA *env_vca = nullptr;

	Unigine::Math::Vec3 velocity;
	Unigine::Math::Vec3 start_point = Unigine::Math::Vec3(-5, 80, 0);

	SampleDescriptionWindow sample_description_window;
	Unigine::WidgetSliderPtr engine_slider;
	Unigine::WidgetSliderPtr wind_forest_slider;
	Unigine::WidgetSliderPtr rain_forest_slider;
	Unigine::WidgetSliderPtr cover_forest_slider;
	Unigine::WidgetSliderPtr env_vca_slider;
	Unigine::WidgetSliderPtr doppler_rpm_slider;
	Unigine::WidgetSliderPtr doppler_velocity_slider;
	Unigine::WidgetCheckBoxPtr show_doppler_box_check_box;
};