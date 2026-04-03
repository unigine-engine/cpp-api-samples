// Renders a scene from a separate camera into an offscreen texture.
// The player camera is rendered each frame using a dedicated viewport,
// and the resulting texture can be applied to any material.

#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

// Captures a player camera's view to a renderable texture each frame
// and assigns it as the albedo texture of the attached object's material.
class CameraToTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CameraToTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Player whose camera view is rendered to texture
	PROP_PARAM(Node, player_camera, "", "", "", "filter=Player");

private:
	void init();
	void update();

private:
	Unigine::PlayerPtr player;
	// Render target texture that receives the camera output
	Unigine::TexturePtr texture;
	// Dedicated viewport for offscreen rendering
	Unigine::ViewportPtr viewport;
};
