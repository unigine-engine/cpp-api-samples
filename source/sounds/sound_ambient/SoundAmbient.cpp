#include "SoundAmbient.h"

REGISTER_COMPONENT(SoundAmbient)


using namespace Unigine;

void SoundAmbient::init()
{
	sample_description_window.createWindow();

	String file_path = sound_file.get();
	if (file_path.empty())
	{
		Log::error("SoundSourceController::init(): Sound File path is empty\n");
		return;
	}

	ambient_source = AmbientSource::create(file_path);
	if (!ambient_source)
	{
		Log::error("SoundAmbient::init(): Sound File is not valid\n");
		return;
	}

	ambient_source->setLoop(1);
	ambient_source->setGain(1.f);
	ambient_source->play();

	// create settings UI
	auto parameters = sample_description_window.getParameterGroupBox();

	WidgetButtonPtr button_play = WidgetButton::create("Play");
	WidgetButtonPtr button_stop = WidgetButton::create("Stop");

	button_play->getEventClicked().connect(*this, [this]() { ambient_source->play(); });
	button_stop->getEventClicked().connect(*this, [this]() { ambient_source->stop(); });

	WidgetHBoxPtr hbox_buttons = WidgetHBox::create(10, 0);
	hbox_buttons->addChild(button_play);
	hbox_buttons->addChild(button_stop);
	parameters->addChild(hbox_buttons, Gui::ALIGN_LEFT);

	sample_description_window.addBoolParameter("Loop:", "Loop", ambient_source->getLoop(),
		[this](bool activate) { ambient_source->setLoop(activate ? 1 : 0); });

	sample_description_window.addBoolParameter("Stream:", "Stream", is_stream,
		[this](bool active) { change_souce_type(); });

	sample_description_window.addFloatParameter("Gain:", "Gain", ambient_source->getGain(), 0.0f,
		1.0f, [this](float val) { ambient_source->setGain(val); });

	sample_description_window.addFloatParameter("Pitch:", "Pitch", ambient_source->getPitch(), 0.1f,
		5.0f, [this](float val) { ambient_source->setPitch(val); });
}

void SoundAmbient::shutdown()
{
	ambient_source.deleteLater();
	sample_description_window.shutdown();
}

void SoundAmbient::change_souce_type()
{
	int is_loop = ambient_source->getLoop();
	bool is_playing = ambient_source->isPlaying();
	float gain = ambient_source->getGain();
	float pitch = ambient_source->getPitch();

	ambient_source.deleteLater();

	is_stream = !is_stream;
	if (is_stream)
		ambient_source = AmbientSource::create(sound_file, 1);
	else
		ambient_source = AmbientSource::create(sound_file, 0);

	ambient_source->setLoop(is_loop);
	ambient_source->setGain(gain);
	ambient_source->setPitch(pitch);

	if (is_playing)
		ambient_source->play();
	else
		ambient_source->stop();
}
