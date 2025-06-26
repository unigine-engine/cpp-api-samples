#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

class CameraToTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CameraToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, player_camera, "", "", "", "filter=Player");

private:
	void init();
	void update();

private:
	Unigine::PlayerPtr player;
	Unigine::TexturePtr texture;
	Unigine::ViewportPtr viewport;
};
