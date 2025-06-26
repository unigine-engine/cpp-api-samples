#pragma once

#include <UniginePlayers.h>
#include <UnigineWidgets.h>
#include <UnigineVector.h>

class ViewportCube final
{
public:
	ViewportCube(const Unigine::PlayerPtr &player, float ortho_width, float ortho_height);
	~ViewportCube();

	bool isHovered() const { return is_hovered; }

	void setViewcubePosition(const Unigine::Math::ivec2 &pos);
	Unigine::Math::ivec2 getViewcubePosition() const;

	void setChangeProjectionEnabled(bool enabled);
	bool getChangeProjectionEnabled() const;

	void setCenteringPosition(const Unigine::Math::Vec3 &pos) { centering_position = pos; }
	Unigine::Math::Vec3 getCenteringPosition() const { return centering_position; }

	void setMinimalDistance(Unigine::Math::Scalar dist) { min_distance = dist; }
	Unigine::Math::Scalar getMinimalDistance() const { return min_distance; }

	void update();

private:
	void ortho_toggled();

	static Unigine::Math::vec3 get_polygon_pos(const char *name);
	static Unigine::Math::quat get_polygon_rot(const Unigine::Math::vec3 &pos);

	void animate_rotation(const Unigine::Math::quat &rot);
	void update_rotation();

private:
	Unigine::Vector<Unigine::Math::quat> rotations;
	Unigine::Vector<Unigine::Math::vec3> positions;

	Unigine::Math::mat4 perspective_transform;
	Unigine::Math::mat4 ortho_transform;

	bool is_hovered = false;

	Unigine::WidgetHBoxPtr main_widget;
	Unigine::WidgetHBoxPtr ortho_hbox;
	Unigine::WidgetIconPtr ortho_icon;
	Unigine::WidgetCanvasPtr cube_canvas;
	Unigine::WidgetCanvasPtr arrows_canvas;
	Unigine::EventConnection icon_connection;

	Unigine::PlayerPtr player;

	Unigine::Math::Vec3 centering_position;

	Unigine::Math::quat old_rotation;
	Unigine::Math::quat new_rotation;

	Unigine::Math::Vec3 old_position;
	Unigine::Math::Vec3 new_position;

	Unigine::String viewcube_texture_touch_path;
	Unigine::String viewcube_texture_untouch_path;

	float ortho_width = 0;
	float ortho_height = 0;
	bool ortho_enabled = false;

	float timer_rotation = 0.0f;

	Unigine::Math::Scalar min_distance = 0;
};
