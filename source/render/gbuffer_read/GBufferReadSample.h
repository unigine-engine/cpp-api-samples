#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class GBufferRead;
class GBufferReadSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferReadSample, ComponentBase)

	COMPONENT_INIT(init, 10)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, node_to_render)

	PROP_PARAM(Node, roughness_node_param, "Roughness Display")
	PROP_PARAM(Node, metallness_node_param, "Metalness Display")
	PROP_PARAM(Node, albedo_node_param, "Albedo Display")
	PROP_PARAM(Node, normal_node_param, "Normal Display")
	PROP_PARAM(Node, depth_node_param, "Depth Display")

private:
	void init();
	void update();
	void shutdown();

	static void init_display(const Unigine::NodePtr &display_node, const Unigine::TexturePtr &texture);

private:
	float phi{0.f};
	const float THETA{20.f};

	GBufferRead *g_buffer_read_component;

	SampleDescriptionWindow sample_description_window;

	bool visualizer_enabled;
};
