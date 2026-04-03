// Lever control element that rotates around a configurable axis.
// Player view direction is projected onto rotation plane to compute angle.
// Supports smooth interpolation toward target angle.

#pragma once
#include "Interactable.h"
#include <UnigineComponentSystem.h>

// Rotates node based on player view direction projected onto rotation plane.
class InteractableLeverComponent
	: public Interactable
	, public Unigine::ComponentBase
{
public:
	COMPONENT(InteractableLeverComponent, ComponentBase);
	PROP_NAME("InteractableLeverComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, Interactable::interactable_update_priority);

	// Local axis around which the lever rotates
	PROP_PARAM(Vec3, rotation_axis, Unigine::Math::vec3(1, 0, 0));
	// Maximum rotation angle from center position in degrees
	PROP_PARAM(Float, rotation_angle_range, 90.0f);
	// Node that receives rotation transform (may differ from component node)
	PROP_PARAM(Node, transformation_node);
	// Whether player can currently manipulate the lever
	PROP_PARAM(Toggle, is_movable, true);

	void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) override;
	float getValue() override;

private:
	void init();
	void update();

	// World position of lever pivot point
	Unigine::Math::vec3 lever_position;
	// Point where player grabbed the lever
	Unigine::Math::vec3 grab_point;
	// Direction from lever to player position
	Unigine::Math::vec3 player_vec;

	// Current rotation direction vector on rotation plane
	Unigine::Math::vec3 rotation_vec;
	// Target rotation direction based on player view
	Unigine::Math::vec3 rotation_target_vec;
	// Previous frame rotation vector for interpolation
	Unigine::Math::vec3 rotation_vec_old;
	// X-axis of the rotation plane (perpendicular to rotation axis)
	Unigine::Math::vec3 valve_plane_x;
	// Y-axis of the rotation plane (perpendicular to rotation axis)
	Unigine::Math::vec3 valve_plane_y;
	// Normal of rotation plane (equals rotation axis direction)
	Unigine::Math::vec3 valve_plane_normal;

	// Direction player is looking in world space
	Unigine::Math::vec3 player_view_direction;
	// Player camera position in world space
	Unigine::Math::vec3 player_position;

	// Initial lever direction when not grabbed
	Unigine::Math::vec3 lever_direction_off;
	// Initial rotation stored for delta calculations
	Unigine::Math::quat initial_rotation;

	// Current lever rotation angle in degrees
	float rotation_angle = 0.0f;
	// Target angle computed from player view projection
	float rotation_angle_target = 0.0f;
	// Previous frame target angle for smoothing
	float rotation_angle_target_old = 0.0f;

	// Tracks if lever was being used previous frame
	bool used_last_frame = false;
	// Tracks if lever is being used current frame
	bool used_current_frame = false;
};
