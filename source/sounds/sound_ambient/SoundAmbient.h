#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>


class SoundAmbient : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SoundAmbient, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(File, sound_file);

private:
	void init();
	void shutdown();

	void change_souce_type();

private:
	Unigine::AmbientSourcePtr ambient_source;
	bool is_stream = false;

	SampleDescriptionWindow sample_description_window;
};
