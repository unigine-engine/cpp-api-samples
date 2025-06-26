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

	PROP_PARAM(Node, camera_top, "", "", "", "filter=Player");
	PROP_PARAM(Node, camera_bottom, "", "", "", "filter=Player");
	PROP_PARAM(Node, display_player1, "", "", "", "filter=ObjectMeshStatic");
	PROP_PARAM(Node, display_player2, "", "", "", "filter=ObjectMeshStatic");

private:
	void init();
	void update();
	void shutdown();
	void update_screen_size();
	void create_texture_2D(Unigine::Texture &texture);
	void init_gui();
	void create_about();
	Unigine::WidgetLabelPtr setup_label(Unigine::GuiPtr gui, const char *text);
	void update_labels_pos();
	static void bind_texture(const Unigine::NodePtr &tv_display,
		const Unigine::TexturePtr &texture);

private:
	Unigine::WidgetSpritePtr widget_top;
	Unigine::WidgetSpritePtr widget_bottom;
	Unigine::CameraPtr cached_camera_top;
	Unigine::CameraPtr cached_camera_bottom;
	Unigine::TexturePtr texture_top;
	Unigine::TexturePtr texture_bottom;
	Unigine::ViewportPtr viewport_top;
	Unigine::ViewportPtr viewport_bottom;
	Unigine::WidgetLabelPtr wasd_label;
	Unigine::WidgetLabelPtr arrows_label;
	Unigine::WidgetVBoxPtr lay;
	int current_height;
	int current_width;
};
