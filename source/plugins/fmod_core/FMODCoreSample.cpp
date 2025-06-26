#include "FMODCoreSample.h"

#include <UnigineVisualizer.h>
#include <UniginePrimitives.h>

REGISTER_COMPONENT(FMODCoreSample);

using namespace Unigine;
using namespace Math;
using namespace Unigine::Plugins::FMOD;

void FMODCoreSample::init()
{
	sample_description_window.createWindow(Gui::ALIGN_LEFT, 500);

	// load the FMOD plugin
	if(Engine::get()->findPlugin("UnigineFMOD") == -1)
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

	// initialize FMOD Core with 1024 channels in NORMAL mode
	FMODCore *core = FMOD::get()->getCore();
	core->initCore(1024, FMODEnums::NORMAL);

	// load a 2D sound file
	music_sound = core->createSound(
		FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_core/sounds/soundtrack.oga")),
		FMODEnums::_2D);

	// load the same file as a 3D sound
	music_sound_3D = core->createSound(
		FileSystem::getAbsolutePath(FileSystem::resolvePartialVirtualPath("fmod_core/sounds/soundtrack.oga")),
		FMODEnums::_3D);

	// create a sphere to visualize the 3D sound source
	car_sphere = Primitives::createSphere(1.0f);
	car_sphere->setMaterialParameterFloat4("albedo_color", vec4(0.4f, 0.0f, 0.0f, 1.0f), 0);

	Visualizer::setEnabled(true);
	init_description_window();
}

void FMODCoreSample::update()
{
	if (!plugin_initialized)
		return;

	// display the "Music 3D" label above the car sphere in the 3D world
	auto len = music_sound->getLength(FMODEnums::TIME_UNIT::MS);
	Visualizer::renderMessage3D(car_sphere->getWorldPosition(), vec3_zero, "Music 3D", vec4_white, 0, 25);

	// update music progress slider
	unsigned int pos;
	if (music_channel)
	{
		music_channel->getPositionTimeLine(pos, FMODEnums::TIME_UNIT::MS);
		int progress = ftoi(pos / itof(len) * 100);
		if (progress >= 100)
		{
			music_channel->setPositionTimeLine(0, FMODEnums::TIME_UNIT::MS);
			stop_music();
			progress = 0;
		}
		music_position_slider->setValue(progress);
	}
}

void FMODCoreSample::shutdown()
{
	// release all FMOD sounds and channels
	if (music_sound)
	{
		music_sound->release();
		music_sound = nullptr;
	}

	if (music_sound_3D)
	{
		music_sound_3D->release();
		music_sound_3D = nullptr;
	}

	if (music_channel)
	{
		music_channel->release();
		music_channel = nullptr;
	}

	if (music_channel_3D)
	{
		music_channel_3D->release();
		music_channel_3D = nullptr;
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
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

void FMODCoreSample::init_description_window()
{
	// create GUI tabs and controls
	WidgetGroupBoxPtr parameters_groupbox = sample_description_window.getParameterGroupBox();
	WidgetTabBoxPtr tab = WidgetTabBox::create(4, 4);
	parameters_groupbox->addChild(tab, Gui::ALIGN_EXPAND);

	// tab music
	{
		tab->addTab("Music");

		// playback controls
		auto play_button = WidgetButton::create("Play");
		auto stop_button = WidgetButton::create("Stop");
		auto pause_button = WidgetButton::create("Pause/Resume");
		auto plus_button = WidgetButton::create("+ 10 sec");
		auto minus_button = WidgetButton::create("- 10 sec");

		auto hbox = WidgetHBox::create();
		play_button->getEventClicked().connect(this, &FMODCoreSample::play_music);
		stop_button->getEventClicked().connect(this, &FMODCoreSample::stop_music);
		pause_button->getEventClicked().connect(this, &FMODCoreSample::toggle_pause_music);

		minus_button->getEventClicked().connect(this, &FMODCoreSample::minus_ms);
		plus_button->getEventClicked().connect(this, &FMODCoreSample::plus_ms);

		music_position_slider = WidgetSlider::create();

		distortion_slider = WidgetSlider::create();
		distortion_slider->getEventChanged().connect(this, &FMODCoreSample::distortion_changed);

		volume_slider = WidgetSlider::create();
		volume_slider->getEventChanged().connect(this, &FMODCoreSample::volume_changed);
		volume_slider->setValue(100);
		hbox->addChild(minus_button);
		hbox->addChild(plus_button);

		tab->addChild(WidgetLabel::create("Time Line"), Gui::ALIGN_EXPAND);
		tab->addChild(music_position_slider, Gui::ALIGN_EXPAND);


		tab->addChild(hbox, Gui::ALIGN_EXPAND);
		tab->addChild(play_button, Gui::ALIGN_EXPAND);
		tab->addChild(stop_button, Gui::ALIGN_EXPAND);
		tab->addChild(pause_button, Gui::ALIGN_EXPAND);

		tab->addChild(WidgetLabel::create("Distortion Mix"), Gui::ALIGN_EXPAND);
		tab->addChild(distortion_slider, Gui::ALIGN_EXPAND);

		tab->addChild(WidgetLabel::create("Volume"), Gui::ALIGN_EXPAND);
		tab->addChild(volume_slider, Gui::ALIGN_EXPAND);
	}

	// tab music 3D
	{
		tab->addTab("Music 3D");
		auto play_button = WidgetButton::create("Play");
		auto stop_button = WidgetButton::create("Stop");
		auto pause_button = WidgetButton::create("Pause/Resume");

		auto hbox = WidgetHBox::create();
		play_button->getEventClicked().connect(this, &FMODCoreSample::play_music_3D);
		stop_button->getEventClicked().connect(this, &FMODCoreSample::stop_music_3D);
		pause_button->getEventClicked().connect(this, &FMODCoreSample::toggle_pause_music_3D);

		tab->addChild(play_button, Gui::ALIGN_EXPAND);
		tab->addChild(stop_button, Gui::ALIGN_EXPAND);
		tab->addChild(pause_button, Gui::ALIGN_EXPAND);
	}

	parameters_groupbox->arrange();
}

void FMODCoreSample::distortion_changed()
{
	if (!music_channel)
	{
		return;
	}

	// adjust distortion effect on the music channel
	music_channel->getDSP(0)->setParameterFloat(0, distortion_slider->getValue() * 0.01f);
}

void FMODCoreSample::volume_changed()
{
	if (!music_channel)
	{
		return;
	}

	// adjust volume on the music channel
	music_channel->setVolume(volume_slider->getValue() * 0.01f);
}

void FMODCoreSample::plus_ms()
{
	if (!music_channel)
	{
		return;
	}

	// jump forward by 10 seconds in the timeline
	unsigned int curr_time_line;
	unsigned int len = music_sound->getLength(FMODEnums::TIME_UNIT::MS);
	music_channel->getPositionTimeLine(curr_time_line, FMODEnums::TIME_UNIT::MS);
	if (curr_time_line + 10000 >= len)
	{
		music_channel->setPositionTimeLine(0, FMODEnums::TIME_UNIT::MS);
	} else
	{
		music_channel->setPositionTimeLine(curr_time_line + 10000, FMODEnums::TIME_UNIT::MS);
	}
}

void FMODCoreSample::minus_ms()
{
	if (!music_channel)
	{
		return;
	}

	// jump backward by 10 seconds in the timeline
	unsigned int curr_time_line;
	music_channel->getPositionTimeLine(curr_time_line, FMODEnums::TIME_UNIT::MS);
	if (curr_time_line < 10000)
	{
		music_channel->setPositionTimeLine(0, FMODEnums::TIME_UNIT::MS);
	} else
	{
		music_channel->setPositionTimeLine(curr_time_line - 10000, FMODEnums::TIME_UNIT::MS);
	}
}

void FMODCoreSample::play_music()
{
	// start 2D music playback if 3D music is not playing
	// adds a distortion DSP effect and sets the volume
	if (!music_channel_3D || !music_channel_3D->isPlaying())
	{
		stop_music();

		music_channel = music_sound->play();
		music_channel->addDSP(0, DSPType::DISTORTION)->setParameterFloat(0, distortion_slider->getValue() * 0.01f);
		music_channel->setVolume(volume_slider->getValue() * 0.01f);
	}
}

void FMODCoreSample::stop_music()
{
	if (!music_channel)
	{
		return;
	}

	music_channel->stop();
	music_channel = nullptr;
}

void FMODCoreSample::toggle_pause_music()
{
	if (!music_channel)
	{
		return;
	}

	music_channel->setPaused(!music_channel->isPaused());
}

void FMODCoreSample::play_music_3D()
{
	// start 3D music playback at the car_sphere position
	if (!music_channel || !music_channel->isPlaying())
	{
		stop_music_3D();

		music_channel_3D = music_sound_3D->play();
		music_channel_3D->setPosition(car_sphere->getWorldPosition());
	}
}

void FMODCoreSample::stop_music_3D()
{
	if (!music_channel_3D)
	{
		return;
	}

	music_channel_3D->stop();
	music_channel_3D = nullptr;
}

void FMODCoreSample::toggle_pause_music_3D()
{
	if (!music_channel_3D)
	{
		return;
	}

	music_channel_3D->setPaused(!music_channel_3D->isPaused());
}
