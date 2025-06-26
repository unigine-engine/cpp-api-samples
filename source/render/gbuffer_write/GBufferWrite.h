#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class GBufferWrite : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferWrite, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Material, modify_buffer_material)

private:
	void init();
	void update();
	void shutdown();

	void g_buffers_ready_callback();

private:
	float influence{0.f};
	float plastic{0.f};
	Unigine::Math::vec4 color{Unigine::Math::vec4_one};

	SampleDescriptionWindow sample_description_window;

	Unigine::WidgetLabelPtr plastic_label;
	Unigine::WidgetLabelPtr influence_label;
	Unigine::WidgetLabelPtr color_label;
};
