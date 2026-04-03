// Prevents first-person weapon from clipping through walls by rendering
// it in a separate pass with a custom near clip plane. Uses a dedicated
// weapon camera that follows the main camera's orientation.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"

#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

class WeaponClipping : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(WeaponClipping, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

	// Main scene camera
	PROP_PARAM(Node, camera, "", "", "", "filter=Player");
	// Separate camera for weapon rendering with different clipping
	PROP_PARAM(Node, weapon_camera, "", "", "", "filter=Player");

	Unigine::ViewportPtr &getRenderViewport() {
		return viewport;
	}

private:
	void init();
	void update();
	void post_update();
	void shutdown();
	// Handles window resize for texture recreation
	void update_screen_size();
	// Creates render target texture matching screen dimensions
	void create_texture_2D(Unigine::Texture &in_texture);

	// Callback for compositing weapon layer onto main render
	void render_callback();

private:
	Unigine::PlayerPtr player;
	// Player for weapon-only rendering pass
	Unigine::PlayerPtr weapon_player;
	// Viewport for weapon layer rendering
	Unigine::ViewportPtr viewport;
	// Texture containing rendered weapon layer
	Unigine::TexturePtr texture;

	// Cached screen dimensions for resize detection
	int current_width{ 0 };
	int current_height{ 0 };

	// Flag to prevent recursive weapon rendering
	bool is_rendering_weapon{false};
	// Stored mouse handle mode for restoration
	Unigine::Input::MOUSE_HANDLE init_handle;
};
