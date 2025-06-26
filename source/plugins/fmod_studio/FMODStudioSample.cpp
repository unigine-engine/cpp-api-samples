#include "FMODStudioSample.h"

#include <UnigineVisualizer.h>
#include <UniginePrimitives.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(FMODStudioSample);

using namespace Unigine;
using namespace Math;
using namespace Unigine::Plugins::FMOD;

void FMODStudioSample::init()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 500);

	// load the FMOD plugin
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

	// create two spheres: one static (car) and one moving (doppler)
	car_sphere = Primitives::createSphere(2.0f);
	car_sphere->setMaterialParameterFloat4("albedo_color", vec4(0.4f, 0.0f, 0.0f, 1.0f), 0);

	doppler_sphere = Primitives::createSphere(1.0f);
	doppler_sphere->setMaterialParameterFloat4("albedo_color", vec4(0.0f, 4.0f, 0.0f, 1.0f), 0);
	doppler_sphere->setWorldPosition(start_point);

	// initialize FMOD Studio and load sound banks
	FMODStudio *studio = FMOD::get()->getStudio();
	studio->useStudioLiveUpdateFlag();
	studio->initStudio();
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Master.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Master.strings.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/Vehicles.bank")));
	studio->loadBank(FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_studio/fmod_banks/SFX.bank")));

	// play ambient forest and engine events
	forest_event = studio->getEvent("event:/Ambience/Forest");
	forest_event->play();

	engine_event = studio->getEvent("event:/Vehicles/Car Engine");
	engine_event->play();

	// doppler effect setup
	doppler_engine_event = studio->getEvent("event:/Vehicles/Car Engine");
	doppler_engine_event->setParent(doppler_sphere);
	doppler_engine_event->setParameter("RPM", 4000);

	// initial movement direction
	velocity = -Vec3_forward;

	// load VCA for master environmental volume
	env_vca = studio->getVCA("vca:/Environment");

	Visualizer::setEnabled(true);
	init_description_window();
}

void FMODStudioSample::update()
{
	if (!plugin_initialized)
		return;

	float t = Game::getTime();
	float dt = Game::getIFps();

	// Doppler simulation logic
	if (show_doppler_box_check_box->isChecked())
	{
		doppler_sphere->setEnabled(true);

		// reset position after 2.5 seconds
		if (timer >= 2.5f)
		{
			doppler_sphere->setWorldPosition(start_point);
			timer = 0.0f;
		}
		timer += dt;

		// restart Doppler sound if not playing
		if (!doppler_engine_event->isPlaying() && !doppler_engine_event->isStarting())
		{
			doppler_engine_event->play();
		}

		// move the Doppler object and update sound velocity
		doppler_sphere->setWorldPosition(doppler_sphere->getWorldPosition() + velocity);
		doppler_engine_event->setVelocity(velocity);
		Visualizer::renderMessage3D(doppler_sphere->getWorldPosition(), vec3_zero, "Doppler", vec4_white, 0, 20);
	} else
	{
		doppler_engine_event->stop();
		doppler_sphere->setEnabled(false);
	}

	// show label above the car
	Visualizer::renderMessage3D(car_sphere->getWorldPosition(), vec3_zero, "Car", vec4_white, 0, 20);
}

void FMODStudioSample::shutdown()
{
	// release all FMOD events
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

	// unload the FMOD plugin
	int idx = Engine::get()->findPlugin("UnigineFMOD");
	if (idx != -1)
	{
		Plugin *plugin = Engine::get()->getPluginInterface(idx);
		Engine::get()->destroyPlugin(plugin);
	}
	plugin_initialized = false;

	car_sphere.deleteLater();
	doppler_sphere.deleteLater();
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

void FMODStudioSample::init_description_window()
{
	// create GUI tabs and controls
	WidgetGroupBoxPtr parameters_groupbox = sample_description_window.getParameterGroupBox();
	WidgetTabBoxPtr tab = WidgetTabBox::create(4, 4);
	parameters_groupbox->addChild(tab, Gui::ALIGN_EXPAND);

	// ambience tab
	{
		tab->addTab("Ambience");
		wind_forest_slider = WidgetSlider::create();
		auto wind_label = WidgetLabel::create( "Wind");
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

	// engine tab
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

	// doppler tab
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

	// vca tab
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

void FMODStudioSample::env_vca_slider_changed()
{
	// adjust environment volume
	env_vca->setVolume(env_vca_slider->getValue() * 0.01f);
}

void FMODStudioSample::engine_slider_changed()
{
	// set RPM for engine event
	engine_event->setParameter("RPM", itof(engine_slider->getValue()));
}

void FMODStudioSample::wind_forest_slider_changed()
{
	// set wind intensity in forest ambience
	forest_event->setParameter("Wind", wind_forest_slider->getValue() * 0.01f);
}

void FMODStudioSample::rain_forest_slider_changed()
{
	// set rain intensity in forest ambience
	forest_event->setParameter("Rain", rain_forest_slider->getValue() * 0.01f);
}

void FMODStudioSample::cover_forest_slider_changed()
{
	// set cover parameter in forest ambience
	forest_event->setParameter("Cover", cover_forest_slider->getValue() * 0.01f);
}

void FMODStudioSample::doppler_rpm_slider_changed()
{
	// set RPM for Doppler engine event
	doppler_engine_event->setParameter("RPM", itof(doppler_rpm_slider->getValue()));
}

void FMODStudioSample::doppler_velocity_slider_changed()
{
	// adjust velocity of Doppler object
	velocity.y = -doppler_velocity_slider->getValue() * 0.1f;
}
