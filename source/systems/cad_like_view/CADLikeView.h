#pragma once

#include <UnigineCamera.h>
#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineWidgets.h>

#include <memory>

class ViewportCube;

class CADLikeView : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CADLikeView, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, player_top, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_perspective, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_side, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_front, "", "", "", "filter=Player");

	PROP_PARAM(Float, ortho_width, 3);
	PROP_PARAM(Float, ortho_height, 3);

	PROP_PARAM(Node, figure);

private:
	void create_viewcubes();
	void init();
	void update();
	void shutdown();

	void update_screen_size();
	void set_dragged_player();
	void set_ortho_projection(float width, float height);
	void update_input();
	void create_viewports();
	void update_viewcubes_position();

private:
	Unigine::WidgetSpriteViewportPtr viewport_top;
	Unigine::WidgetSpriteViewportPtr viewport_perspective;
	Unigine::WidgetSpriteViewportPtr viewport_side;
	Unigine::WidgetSpriteViewportPtr viewport_front;

	Unigine::CameraPtr camera_top;
	Unigine::CameraPtr camera_perspective;
	Unigine::CameraPtr camera_side;
	Unigine::CameraPtr camera_front;

	std::unique_ptr<ViewportCube> viewcube_top;
	std::unique_ptr<ViewportCube> viewcube_perspective;
	std::unique_ptr<ViewportCube> viewcube_side;
	std::unique_ptr<ViewportCube> viewcube_front;

	Unigine::PlayerPtr dragged_player;

	Unigine::WidgetGridBoxPtr lay;

	int current_width;
	int current_height;
};
