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

	PROP_PARAM(Node, camera, "", "", "", "filter=Player");
	PROP_PARAM(Node, weapon_camera, "", "", "", "filter=Player");

	Unigine::ViewportPtr &getRenderViewport() {
		return viewport;
	}

private:
	void init();
	void update();
	void post_update();
	void shutdown();
	void update_screen_size();
	void create_texture_2D(Unigine::Texture &in_texture);

	void render_callback();

private:
	Unigine::PlayerPtr player;
	Unigine::PlayerPtr weapon_player;
	Unigine::ViewportPtr viewport;
	Unigine::TexturePtr texture;

	int current_width{ 0 };
	int current_height{ 0 };

	bool is_rendering_weapon{false};
	Unigine::Input::MOUSE_HANDLE init_handle;
};
