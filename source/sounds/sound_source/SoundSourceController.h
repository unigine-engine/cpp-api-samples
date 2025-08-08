#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>

class SoundSourceController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundSourceController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, sound_file);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::SoundSourcePtr sound_source;

	SampleDescriptionWindow sample_description_window;
};
