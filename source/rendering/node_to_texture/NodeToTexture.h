// Renders a single node to an offscreen texture using a custom viewport.
// The node is rendered from a specified camera position, and the result
// is applied to an object's material for in-world display.

#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

// Captures isolated node rendering to texture for previews or thumbnails.
class NodeToTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Camera used for rendering the node
	PROP_PARAM(Node, player_camera, "", "", "", "filter=Player");
	// Target node to be rendered to texture
	PROP_PARAM(Node, node_to_render);
	// Object that will display the rendered texture
	PROP_PARAM(Node, object_view, "", "", "", "filter=Object");

private:
	void init();
	void update();

private:
	Unigine::PlayerPtr player;
	// Render target texture receiving the node image
	Unigine::TexturePtr texture;
	// Environment texture for reflections in offscreen render
	Unigine::TexturePtr env_texture;
	// Dedicated viewport for single-node rendering
	Unigine::ViewportPtr viewport;
};
