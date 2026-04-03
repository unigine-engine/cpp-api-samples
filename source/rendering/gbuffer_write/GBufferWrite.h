// Demonstrates modifying G-buffer data after the deferred pass.
// Applies custom material properties (color, plastic effect) to
// the entire screen by writing to G-buffer textures.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

// Modifies G-buffer content after the deferred pass by applying color tint
// and plastic material effects to the entire screen using MRT rendering.
class GBufferWrite : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferWrite, ComponentBase)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// Material with shader that modifies G-buffer data
	PROP_PARAM(Material, modify_buffer_material)

private:
	void init();
	void update();
	void shutdown();

	// Called when G-buffer is ready for modification
	void g_buffers_ready_callback();

private:
	// Effect blend factor (0 = no effect, 1 = full effect)
	float influence{0.f};
	// Plastic material effect intensity
	float plastic{0.f};
	// Tint color applied to G-buffer albedo
	Unigine::Math::vec4 color{Unigine::Math::vec4_one};

	SampleDescriptionWindow sample_description_window;

	// UI labels displaying current parameter values
	Unigine::WidgetLabelPtr plastic_label;
	Unigine::WidgetLabelPtr influence_label;
	Unigine::WidgetLabelPtr color_label;
};
