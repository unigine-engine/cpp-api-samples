#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

class NodeToTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(NodeToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, player_camera, "", "", "", "filter=Player");
	PROP_PARAM(Node, node_to_render);
	PROP_PARAM(Node, object_view, "", "", "", "filter=Object");

private:
	void init();
	void update();

private:
	Unigine::PlayerPtr player;
	Unigine::TexturePtr texture;
	Unigine::TexturePtr env_texture;
	Unigine::ViewportPtr viewport;
};
