// Creates a split-screen layout with two camera views rendered to textures.
// The top and bottom viewports are displayed both as screen widgets and
// on in-world TV display objects for a racing game setup.

#pragma once

#include <UnigineCamera.h>
#include <UnigineComponentSystem.h>
#include <UnigineViewport.h>
#include <UnigineWidgets.h>

class SplitScreenTexture : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SplitScreenTexture, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Player cameras for top and bottom views
	PROP_PARAM(Node, camera_top, "", "", "", "filter=Player");
	PROP_PARAM(Node, camera_bottom, "", "", "", "filter=Player");
	// In-world display objects for each player's view
	PROP_PARAM(Node, display_player1, "", "", "", "filter=ObjectMeshStatic");
	PROP_PARAM(Node, display_player2, "", "", "", "filter=ObjectMeshStatic");

private:
	void init();
	void update();
	void shutdown();
	// Handles window resize events
	void update_screen_size();
	// Creates render target texture with current screen dimensions
	void create_texture_2D(Unigine::Texture &texture);
	void init_gui();
	void create_about();
	Unigine::WidgetLabelPtr setup_label(Unigine::GuiPtr gui, const char *text);
	void update_labels_pos();
	// Assigns rendered texture to display object's material
	static void bind_texture(const Unigine::NodePtr &tv_display,
		const Unigine::TexturePtr &texture);

private:
	// Screen widgets displaying each viewport
	Unigine::WidgetSpritePtr widget_top;
	Unigine::WidgetSpritePtr widget_bottom;
	// Cameras extracted from player nodes
	Unigine::CameraPtr cached_camera_top;
	Unigine::CameraPtr cached_camera_bottom;
	// Render target textures for each view
	Unigine::TexturePtr texture_top;
	Unigine::TexturePtr texture_bottom;
	// Viewports for offscreen rendering
	Unigine::ViewportPtr viewport_top;
	Unigine::ViewportPtr viewport_bottom;
	// Control hint labels
	Unigine::WidgetLabelPtr wasd_label;
	Unigine::WidgetLabelPtr arrows_label;
	Unigine::WidgetVBoxPtr lay;
	// Cached window dimensions for resize detection
	int current_height;
	int current_width;
};
