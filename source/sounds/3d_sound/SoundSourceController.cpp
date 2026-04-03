// Demonstrates 3D positional sound using SoundSource. Audio is spatialized based
// on listener distance with configurable min/max distance falloff. Supports
// streaming mode for large files and visualizes the sound source location.

#include "SoundSourceController.h"

#include <UnigineVisualizer.h>

REGISTER_COMPONENT(SoundSourceController)

using namespace Unigine;


// 3D sound source is created and UI controls are set up.
void SoundSourceController::init()
{
	sample_description_window.createWindow();

	String file_path = sound_file.get();
	if (file_path.empty())
	{
		Log::error("SoundSourceController::init(): Sound File path is empty\n");
		return;
	}

	// Create positional sound source from file
	sound_source = SoundSource::create(file_path.get());
	if (!sound_source)
	{
		Log::error("SoundSourceController::init(): Sound File is not valid\n");
		return;
	}

	// Configure distance falloff: full volume at 5m, silent at 50m
	sound_source->setMinDistance(5.0f);
	sound_source->setMaxDistance(50.0f);
	sound_source->setLoop(1);
	sound_source->play();

	// Create settings UI
	auto parameters = sample_description_window.getParameterGroupBox();

	WidgetButtonPtr button_play = WidgetButton::create("Play");
	WidgetButtonPtr button_stop = WidgetButton::create("Stop");

	button_play->getEventClicked().connect(*this, [this]() { sound_source->play(); });
	button_stop->getEventClicked().connect(*this, [this]() { sound_source->stop(); });

	WidgetHBoxPtr hbox_buttons = WidgetHBox::create(10, 0);
	hbox_buttons->addChild(button_play);
	hbox_buttons->addChild(button_stop);
	parameters->addChild(hbox_buttons, Gui::ALIGN_LEFT);

	sample_description_window.addBoolParameter("Loop:", "Loop", sound_source->getLoop(),
		[this](bool activate) { sound_source->setLoop(activate ? 1 : 0); });

	sample_description_window.addBoolParameter("Stream:", "Stream", sound_source->isStream(),
		[this](bool active) { sound_source->setStream(active); });

	sample_description_window.addFloatParameter("Gain:", "Gain", sound_source->getGain(), 0.0f,
		1.0f, [this](float val) { sound_source->setGain(val); });

	sample_description_window.addFloatParameter("Pitch:", "Pitch", sound_source->getPitch(), 0.1f,
		5.0f, [this](float val) { sound_source->setPitch(val); });

	Visualizer::setEnabled(true);
}

// Sound source distance spheres are visualized.
void SoundSourceController::update()
{
	if (!sound_source)
		return;

	sound_source->renderVisualizer();
}

// Visualizer is disabled and UI is cleaned up.
void SoundSourceController::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}
