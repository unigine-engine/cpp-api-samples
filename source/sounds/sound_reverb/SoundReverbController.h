#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>


class SoundReverbController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundReverbController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, sound_node, "Sound Source");

private:
	void init();
	void update();
	void shutdown();

	void update_reverb_settings();

private:
	Unigine::SoundSourcePtr sound_source;

	Unigine::SoundReverbPtr sound_reverb;
	float reverb_power = 0.5f;

	SampleDescriptionWindow sample_description_window;
};
