// Valve control element that rotates around its Z-axis based on player grab motion.
// The grab point tracks player view direction projected onto the valve plane.
// Rotation angle is computed from the angular difference between successive grab positions.

#include "InteractableValveComponent.h"

#include "UnigineVisualizer.h"
REGISTER_COMPONENT(InteractableValveComponent);

using namespace Unigine;
using namespace Math;

// Threshold for stopping interpolation when target angle is nearly reached
const float angle_eps = 0.05f;

// Valve plane axes are captured from transformation node orientation.
void InteractableValveComponent::init()
{
	// Default to component node if no explicit transformation target
	if (!transformation_node)
	{
		transformation_node = node;
	}

	// Build orthonormal basis for the valve rotation plane
	valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_X).normalize();
	valve_plane_y = (vec3)transformation_node->getWorldDirection(AXIS_Y).normalize();
	valve_plane_normal = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();

	// Initialize with default view and grab at valve center
	player_view_direction = valve_plane_y;
	grab_point = (vec3)transformation_node->getWorldPosition();
}

// Grab point is captured and projected onto valve plane for initial frame.
void InteractableValveComponent::use(Unigine::PlayerPtr player,
	Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	player_view_direction = player->getViewDirection();
	player_position = (vec3)player->getWorldPosition();

	grab_point = intersection_point;
	// On first grab frame, capture initial grab direction on valve plane
	if (!used_last_frame)
	{
		vec3 grab_vec = (grab_point - (vec3)transformation_node->getWorldPosition());
		grab_vec_old = valve_plane_x * dot(valve_plane_x, grab_vec)
			+ valve_plane_y * dot(valve_plane_y, grab_vec);
	}
}

// Angular delta between grab positions drives rotation toward target angle.
void InteractableValveComponent::update()
{
	// Recalculate valve plane basis each frame for movable valves
	if (is_movable)
	{
		valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_X).normalize();
		valve_plane_y = (vec3)transformation_node->getWorldDirection(AXIS_Y).normalize();
		valve_plane_normal = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();
	}
	// Project current grab position onto valve plane
	if (used_current_frame)
	{
		vec3 valve_position = (vec3)transformation_node->getWorldPosition();
		// Extend player view ray to valve distance
		grab_vec = (player_position
					   + player_view_direction * (player_position - valve_position).length())
			- valve_position;

		// Project onto valve plane to get rotation direction
		rotation_vec = valve_plane_x * dot(valve_plane_x, grab_vec)
			+ valve_plane_y * dot(valve_plane_y, grab_vec);
		rotation_vec.normalize();
	}
	// Smoothly interpolate toward target while interacting or settling
	if (Math::abs(rotation_angle - rotation_angle_target) > angle_eps || used_current_frame)
	{
		// Accumulate angular delta between successive grab directions
		rotation_angle_target += getAngle(vec3(grab_vec_old), vec3(rotation_vec),
			vec3(valve_plane_normal));
		rotation_angle_target = clamp(rotation_angle_target, 0.0f, rotation_angle_range);
		rotation_angle = lerp(rotation_angle, rotation_angle_target, 0.1f);

		transformation_node->setRotation(
			quat(vec3(transformation_node->getDirection(AXIS_Z)), rotation_angle));
	}
	// Store current grab for next frame delta calculation
	grab_vec_old = rotation_vec;
	used_last_frame = used_current_frame;
	used_current_frame = false;
}

// Normalized valve position is returned (0 = start, 1 = full range).
float InteractableValveComponent::getValue()
{
	return rotation_angle / rotation_angle_range;
}
