// CAD-style 2x2 viewport layout manager. Displays four synchronized views
// of the same scene: top, side, front (orthographic) and perspective.
// Each viewport has an orientation viewcube for camera navigation.

#pragma once

#include <UnigineCamera.h>
#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>
#include <UnigineWidgets.h>

#include <memory>

class ViewportCube;

// Creates and manages four viewport widgets in a 2x2 grid layout.
// Handles camera setup, orthographic projection, drag-to-pan input,
// viewport resizing, and viewcube positioning.
class CADLikeView : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CADLikeView, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component creates a CAD-style 2x2 viewport layout with top, perspective, side, and front views, "
							"manages cameras and input, and attaches orientation viewcubes.")

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Players representing each viewport (set in the Editor)
	PROP_PARAM(Node, player_top, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_perspective, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_side, "", "", "", "filter=Player");
	PROP_PARAM(Node, player_front, "", "", "", "filter=Player");

	// Orthographic projection size (set in the Editor)
	PROP_PARAM(Float, ortho_width, 3);
	PROP_PARAM(Float, ortho_height, 3);

	// Scene object to center viewcubes on (set in the Editor)
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
	// Viewport widgets for each view
	Unigine::WidgetSpriteViewportPtr viewport_top;
	Unigine::WidgetSpriteViewportPtr viewport_perspective;
	Unigine::WidgetSpriteViewportPtr viewport_side;
	Unigine::WidgetSpriteViewportPtr viewport_front;

	// Cameras corresponding to each player
	Unigine::CameraPtr camera_top;
	Unigine::CameraPtr camera_perspective;
	Unigine::CameraPtr camera_side;
	Unigine::CameraPtr camera_front;

	// Viewcube overlays for orientation
	std::unique_ptr<ViewportCube> viewcube_top;
	std::unique_ptr<ViewportCube> viewcube_perspective;
	std::unique_ptr<ViewportCube> viewcube_side;
	std::unique_ptr<ViewportCube> viewcube_front;

	// Currently dragged player for interactive movement
	Unigine::PlayerPtr dragged_player;

	// Layout container for viewports
	Unigine::WidgetGridBoxPtr lay;

	// Cached window dimensions to handle resizing
	int current_width;
	int current_height;

	Unigine::Input::MOUSE_HANDLE last_handle;
};
