// Valve control element that rotates around its Z-axis based on player grab motion.
// Rotation is driven by tracking angular movement of player view on valve plane.
// Supports smooth interpolation and configurable rotation range.

#pragma once
#include "Interactable.h"

#include <UnigineComponentSystem.h>

// Rotates based on player grab motion projected onto the valve plane.
class InteractableValveComponent
	: public Interactable
	, public Unigine::ComponentBase
{
public:
	COMPONENT(InteractableValveComponent, ComponentBase);
	PROP_NAME("InteractableValveComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, Interactable::interactable_update_priority);

	// Maximum rotation angle from start position in degrees
	PROP_PARAM(Float, rotation_angle_range, 90.0f);
	// Node that receives rotation transform
	PROP_PARAM(Node, transformation_node);
	// Whether player can currently manipulate the valve
	PROP_PARAM(Toggle, is_movable, true);


	void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) override;
	float getValue() override;

private:
	void init();
	void update();

	// X-axis of the valve rotation plane
	Unigine::Math::vec3 valve_plane_x;
	// Y-axis of the valve rotation plane
	Unigine::Math::vec3 valve_plane_y;
	// Normal of valve plane (Z-axis, rotation axis)
	Unigine::Math::vec3 valve_plane_normal;

	// Direction player is looking in world space
	Unigine::Math::vec3 player_view_direction;
	// Player camera position in world space
	Unigine::Math::vec3 player_position;
	// Current grab direction projected onto valve plane
	Unigine::Math::vec3 rotation_vec;

	// Direction from valve center to current grab position
	Unigine::Math::vec3 grab_vec;
	// World position where player grabbed the valve
	Unigine::Math::vec3 grab_point;
	// Previous frame grab direction for delta calculation
	Unigine::Math::vec3 grab_vec_old;
	// Current valve rotation angle in degrees
	float rotation_angle = 0.0f;
	// Target angle computed from grab motion
	float rotation_angle_target = 0.0f;

	// Tracks if valve was being used previous frame
	bool used_last_frame = false;
	// Tracks if valve is being used current frame
	bool used_current_frame = false;
};
