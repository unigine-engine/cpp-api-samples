// Interactive 3D orientation cube widget for camera navigation. Displays a
// clickable cube that rotates with the camera; clicking faces/edges/corners
// animates camera to that viewpoint. Supports ortho/perspective toggle.

#pragma once

#include <UniginePlayers.h>
#include <UnigineWidgets.h>
#include <UnigineVector.h>

// Orientation cube overlay for viewport navigation. Renders a 3D cube that
// mirrors camera orientation. Clicking cube elements triggers smooth camera
// rotation animation. Also displays navigation arrows when axis-aligned.
class ViewportCube final
{
public:
	ViewportCube(const Unigine::PlayerPtr &player, float ortho_width, float ortho_height);
	~ViewportCube();

	// Returns true if the mouse is currently over the cube widget
	bool isHovered() const { return is_hovered; }

	// Cube widget position in screen space
	void setViewcubePosition(const Unigine::Math::ivec2 &pos);
	Unigine::Math::ivec2 getViewcubePosition() const;

	// Enable/disable the projection toggle (ortho/perspective)
	void setChangeProjectionEnabled(bool enabled);
	bool getChangeProjectionEnabled() const;

	// Camera centering position for rotation animation
	void setCenteringPosition(const Unigine::Math::Vec3 &pos) { centering_position = pos; }
	Unigine::Math::Vec3 getCenteringPosition() const { return centering_position; }

	// Minimal camera distance used during rotation animation
	void setMinimalDistance(Unigine::Math::Scalar dist) { min_distance = dist; }
	Unigine::Math::Scalar getMinimalDistance() const { return min_distance; }

	void update();

private:
	// Switch between perspective and orthographic projection
	void ortho_toggled();

	// Convert mesh surface name to face position/rotation
	static Unigine::Math::vec3 get_polygon_pos(const char *name);
	static Unigine::Math::quat get_polygon_rot(const Unigine::Math::vec3 &pos);

	// Animate smooth camera rotation toward a cube face or arrow
	void animate_rotation(const Unigine::Math::quat &rot);
	void update_rotation();

private:
	// Cached per-face rotations and positions
	Unigine::Vector<Unigine::Math::quat> rotations;
	Unigine::Vector<Unigine::Math::vec3> positions;

	// Precomputed transforms for ortho and perspective modes
	Unigine::Math::mat4 perspective_transform;
	Unigine::Math::mat4 ortho_transform;

	bool is_hovered = false;

	// UI elements
	Unigine::WidgetHBoxPtr main_widget;
	Unigine::WidgetHBoxPtr ortho_hbox;
	Unigine::WidgetIconPtr ortho_icon;
	Unigine::WidgetCanvasPtr cube_canvas;
	Unigine::WidgetCanvasPtr arrows_canvas;
	Unigine::EventConnection icon_connection;

	Unigine::PlayerPtr player;

	// Camera centering and animation state
	Unigine::Math::Vec3 centering_position;
	Unigine::Math::quat old_rotation;
	Unigine::Math::quat new_rotation;
	Unigine::Math::Vec3 old_position;
	Unigine::Math::Vec3 new_position;

	// Textures for hover and normal states
	Unigine::String viewcube_texture_touch_path;
	Unigine::String viewcube_texture_untouch_path;

	// Projection parameters
	float ortho_width = 0;
	float ortho_height = 0;
	bool ortho_enabled = false;

	// Camera rotation animation timer
	float timer_rotation = 0.0f;

	// Minimal allowed distance for camera animation
	Unigine::Math::Scalar min_distance = 0;
};
