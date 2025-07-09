#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>


class FireHoseSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(FireHoseSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, emitter, "", "", "", "filter=Node");

private:
	void init();
	void update();
	void shutdown();

private:
	bool emitter_rotate = true;
	float time_start = 0.0f;

	SampleDescriptionWindow description_window;
};

