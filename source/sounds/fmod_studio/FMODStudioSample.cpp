// FMOD Studio API integration sample. Demonstrates event-based audio with parameter
// control (ambience, engine RPM), VCA volume groups, and Doppler effect simulation.
// A moving sphere demonstrates velocity-based pitch shifting for realistic motion audio.

#include "FMODStudioSample.h"

#include <UnigineVisualizer.h>
#include <UniginePrimitives.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(FMODStudioSample);

using namespace Unigine;
using namespace Math;
using namespace Unigine::Plugins::FMOD;

// FMOD Studio is initialized, banks are loaded, and events are started.
void FMODStudioSample::init()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 500);

	// Load the FMOD plugin
	if (Engine::get()->findPlugin("UnigineFMOD") == -1)
		Engine::get()->addPlugin("UnigineFMOD");
	if (!FMOD::checkPlugin())
	{
		WidgetGroupBoxPtr parameters_groupbox = sample_description_window.getParameterGroupBox();

		auto info_label = WidgetLabel::create();
		info_label->setFontWrap(1);
		info_label->setText("Cannot find FMOD plugin. Please check UnigineFMOD and fmod.dll, fmodL.dll, fmodstudio.dll, fmodstudioL.dll (You can download these files from official site) in bin directory.");

		parameters_groupbox->addChild(info_label);

		return;
	}

	plugin_initialized = true;

	// Create two spheres: red static (car) and green moving (Doppler demo)
	car_sphere = Primitives::createSphere(2.0f);
	car_sphere->setMaterialParameterFloat4("albedo_color", vec4(0.4f, 0.0f, 0.0f, 1.0f), 0);

	doppler_sphere = Primitives::createSphere(1.0f);
	doppler_sphere->setMaterialParameterFloat4("albedo_color", vec4(0.0f, 4.0f, 0.0f, 1.0f), 0);
	doppler_sphere->setWorldPosition(start_point);

	// Initialize FMOD Studio with live update support and load sound banks
	FMODStudio* studio = FMOD::get()->getStudio();
	studio->useStudioLiveUpdateFlag();
	studio->initStudio();
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Master.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Master.strings.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Vehicles.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/SFX.bank")));

	// Start ambient forest loop and engine sound event
	forest_event = studio->getEvent("event:/Ambience/Forest");
	if (forest_event)
		forest_event->play();

	engine_event = studio->getEvent("event:/Vehicles/Car Engine");
	if (engine_event)
		engine_event->play();

	// Configure Doppler demo: engine attached to moving sphere
	doppler_engine_event = studio->getEvent("event:/Vehicles/Car Engine");
	if (doppler_engine_event)
	{
		doppler_engine_event->setParent(doppler_sphere);
		doppler_engine_event->setParameter("RPM", 4000);
	}

	// Set initial movement direction for Doppler object
	velocity = -Vec3_forward;

	// Load VCA for master environment volume control
	env_vca = studio->getVCA("vca:/Environment");

	Visualizer::setEnabled(true);
	init_description_window();
}

// Doppler object is moved and sound velocity is updated.
void FMODStudioSample::update()
{
	if (!plugin_initialized)
		return;

	float dt = Game::getIFps();

	// Doppler simulation: moving sound source demonstrates pitch shift
	if (show_doppler_box_check_box->isChecked())
	{
		doppler_sphere->setEnabled(true);

		// Reset position after 2.5 seconds to loop the demonstration
		if (timer >= 2.5f)
		{
			doppler_sphere->setWorldPosition(start_point);
			timer = 0.0f;
		}
		timer += dt;

		// Restart Doppler sound if stopped
		if (doppler_engine_event && !doppler_engine_event->isPlaying() && !doppler_engine_event->isStarting())
		{
			doppler_engine_event->play();
		}

		// Move sphere and update sound velocity for Doppler pitch calculation
		doppler_sphere->setWorldPosition(doppler_sphere->getWorldPosition() + velocity);
		if (doppler_engine_event)
			doppler_engine_event->setVelocity(velocity);
		Visualizer::renderMessage3D(doppler_sphere->getWorldPosition(), vec3_zero, "Doppler", vec4_white, 0, 20);
	}
	else
	{
		if (doppler_engine_event)
			doppler_engine_event->stop();
		doppler_sphere->setEnabled(false);
	}

	// Display label above static car sphere
	Visualizer::renderMessage3D(car_sphere->getWorldPosition(), vec3_zero, "Car", vec4_white, 0, 20);
}

// All FMOD events and VCA are released, plugin is unloaded.
void FMODStudioSample::shutdown()
{
	// Release all FMOD events
	if (engine_event)
	{
		engine_event->release();
		engine_event = nullptr;
	}

	if (doppler_engine_event)
	{
		doppler_engine_event->release();
		doppler_engine_event = nullptr;
	}

	if (forest_event)
	{
		forest_event->release();
		forest_event = nullptr;
	}

	if (env_vca)
	{
		env_vca->release();
		env_vca = nullptr;
	}

	// Unload the FMOD plugin
	int idx = Engine::get()->findPlugin("UnigineFMOD");
	if (idx != -1)
	{
		Plugin* plugin = Engine::get()->getPluginInterface(idx);
		Engine::get()->destroyPlugin(plugin);
	}
	plugin_initialized = false;

	car_sphere.deleteLater();
	doppler_sphere.deleteLater();
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

// Tabbed UI is created for ambience, engine, Doppler, and VCA controls.
void FMODStudioSample::init_description_window()
{
	// Create GUI tabs and controls
	WidgetGroupBoxPtr parameters_groupbox = sample_description_window.getParameterGroupBox();
	WidgetTabBoxPtr tab = WidgetTabBox::create(4, 4);
	parameters_groupbox->addChild(tab, Gui::ALIGN_EXPAND);

	// Ambience tab - wind, rain, and cover parameters for forest event
	{
		tab->addTab("Ambience");
		wind_forest_slider = WidgetSlider::create();
		auto wind_label = WidgetLabel::create("Wind");
		tab->addChild(wind_label, Gui::ALIGN_EXPAND);
		tab->addChild(wind_forest_slider, Gui::ALIGN_EXPAND);
		wind_forest_slider->getEventChanged().connect(this, &FMODStudioSample::wind_forest_slider_changed);

		rain_forest_slider = WidgetSlider::create();
		auto forest_label = WidgetLabel::create("Rain");
		tab->addChild(forest_label, Gui::ALIGN_EXPAND);
		tab->addChild(rain_forest_slider, Gui::ALIGN_EXPAND);
		rain_forest_slider->getEventChanged().connect(this, &FMODStudioSample::rain_forest_slider_changed);

		cover_forest_slider = WidgetSlider::create();
		auto cover_label = WidgetLabel::create("Cover");
		tab->addChild(cover_label, Gui::ALIGN_EXPAND);
		tab->addChild(cover_forest_slider, Gui::ALIGN_EXPAND);
		cover_forest_slider->getEventChanged().connect(this, &FMODStudioSample::cover_forest_slider_changed);
	}

	// Engine tab - RPM parameter control for car engine event
	{
		tab->addTab("Engine");
		engine_slider = WidgetSlider::create();
		engine_slider->setMinValue(0);
		engine_slider->setMaxValue(8000);
		auto label = WidgetLabel::create("RPM");
		tab->addChild(label, Gui::ALIGN_EXPAND);
		tab->addChild(engine_slider, Gui::ALIGN_EXPAND);
		engine_slider->getEventChanged().connect(this, &FMODStudioSample::engine_slider_changed);
	}

	// Doppler tab - moving sound source with velocity-based pitch shift
	{
		tab->addTab("Doppler");
		show_doppler_box_check_box = WidgetCheckBox::create();
		show_doppler_box_check_box->setChecked(false);
		auto label = WidgetLabel::create("Show Doppler Effect");
		tab->addChild(label, Gui::ALIGN_EXPAND);
		tab->addChild(show_doppler_box_check_box, Gui::ALIGN_EXPAND);
		doppler_rpm_slider = WidgetSlider::create();
		doppler_velocity_slider = WidgetSlider::create();
		doppler_rpm_slider->setMaxValue(8000);
		doppler_rpm_slider->setValue(4000);
		tab->addChild(WidgetLabel::create("RPM"), Gui::ALIGN_EXPAND);
		tab->addChild(doppler_rpm_slider, Gui::ALIGN_EXPAND);

		tab->addChild(WidgetLabel::create("Velocity"), Gui::ALIGN_EXPAND);
		tab->addChild(doppler_velocity_slider, Gui::ALIGN_EXPAND);

		doppler_rpm_slider->getEventChanged().connect(this, &FMODStudioSample::doppler_rpm_slider_changed);
		doppler_velocity_slider->getEventChanged().connect(this, &FMODStudioSample::doppler_velocity_slider_changed);
		doppler_velocity_slider->setValue(5);
	}

	// VCA tab - master volume control for environment sounds
	{
		tab->addTab("VCA");
		env_vca_slider = WidgetSlider::create();
		env_vca_slider->setValue(100);
		auto label = WidgetLabel::create("Sounds Volume");
		tab->addChild(label, Gui::ALIGN_EXPAND);
		tab->addChild(env_vca_slider, Gui::ALIGN_EXPAND);
		env_vca_slider->getEventChanged().connect(this, &FMODStudioSample::env_vca_slider_changed);
	}

	parameters_groupbox->arrange();
}

// VCA volume is set from slider value.
void FMODStudioSample::env_vca_slider_changed()
{
	// Adjust environment volume
	if (env_vca)
		env_vca->setVolume(env_vca_slider->getValue() * 0.01f);
}

// Engine event RPM parameter is updated from slider.
void FMODStudioSample::engine_slider_changed()
{
	// Set RPM for engine event
	if (engine_event)
		engine_event->setParameter("RPM", itof(engine_slider->getValue()));
}

// Forest ambience wind parameter is updated.
void FMODStudioSample::wind_forest_slider_changed()
{
	// Set wind intensity in forest ambience
	if (forest_event)
		forest_event->setParameter("Wind", wind_forest_slider->getValue() * 0.01f);
}

// Forest ambience rain parameter is updated.
void FMODStudioSample::rain_forest_slider_changed()
{
	// Set rain intensity in forest ambience
	if (forest_event)
		forest_event->setParameter("Rain", rain_forest_slider->getValue() * 0.01f);
}

// Forest ambience cover parameter is updated.
void FMODStudioSample::cover_forest_slider_changed()
{
	// Set cover parameter in forest ambience
	if (forest_event)
		forest_event->setParameter("Cover", cover_forest_slider->getValue() * 0.01f);
}

// Doppler engine RPM parameter is updated.
void FMODStudioSample::doppler_rpm_slider_changed()
{
	// Set RPM for Doppler engine event
	if (doppler_engine_event)
		doppler_engine_event->setParameter("RPM", itof(doppler_rpm_slider->getValue()));
}

// Doppler sphere velocity is updated for pitch shifting.
void FMODStudioSample::doppler_velocity_slider_changed()
{
	// Adjust velocity of Doppler object
	velocity.y = -doppler_velocity_slider->getValue() * 0.1f;
}
