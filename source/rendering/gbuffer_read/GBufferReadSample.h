// Sample UI for G-buffer extraction demonstration.
// Renders a rotating node and displays extracted G-buffer components
// (albedo, normal, depth, roughness, metalness) on separate displays.

#pragma once

#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class GBufferRead;

// Demonstrates G-buffer extraction by rendering a rotating node and displaying
// extracted buffer components (albedo, normal, depth, roughness, metalness) on screen.
class GBufferReadSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(GBufferReadSample, ComponentBase)

	// Higher init order ensures GBufferRead is initialized first
	COMPONENT_INIT(init, 10)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// Node that is rendered to extract G-buffer data
	PROP_PARAM(Node, node_to_render)

	// Display nodes for each G-buffer component
	PROP_PARAM(Node, roughness_node_param, "Roughness Display")
	PROP_PARAM(Node, metallness_node_param, "Metalness Display")
	PROP_PARAM(Node, albedo_node_param, "Albedo Display")
	PROP_PARAM(Node, normal_node_param, "Normal Display")
	PROP_PARAM(Node, depth_node_param, "Depth Display")

private:
	void init();
	void update();
	void shutdown();

	// Assigns extracted texture to display node's material
	static void init_display(const Unigine::NodePtr &display_node, const Unigine::TexturePtr &texture);

private:
	// Rotation angle for animated node rendering
	float phi{0.f};
	// Fixed pitch angle for camera positioning
	const float THETA{20.f};

	GBufferRead *g_buffer_read_component;

	SampleDescriptionWindow sample_description_window;

	// Stores original visualizer state for restoration
	bool visualizer_enabled;
};
