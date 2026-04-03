// Lever control element that rotates around a configurable axis.
// Player view direction is projected onto the rotation plane to compute target angle.
// Smooth interpolation is applied each frame until lever reaches target position.

#include "InteractableLeverComponent.h"
#include "UnigineVisualizer.h"
REGISTER_COMPONENT(InteractableLeverComponent);

using namespace Unigine;
using namespace Math;

// Threshold below which angle difference is considered negligible
const float angle_eps = 0.05f;

// Rotation plane axes are computed from transformation node orientation.
void InteractableLeverComponent::init()
{
	// Default to component node if no explicit transformation target
	if (!transformation_node)
		transformation_node = node;

	// Ensure rotation axis is unit length
	rotation_axis = normalize(rotation_axis.get());
	// Build orthonormal basis for the rotation plane
	valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();
	valve_plane_normal = (transformation_node->getWorldRotation() * rotation_axis).normalize();
	valve_plane_y = cross(valve_plane_normal, valve_plane_x).normalize();
	// Store initial orientation for applying relative rotation
	initial_rotation = transformation_node->getWorldRotation();
}

// Player view direction and position are captured for projection calculation.
void InteractableLeverComponent::use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	player_view_direction = player->getViewDirection();
	player_position = (vec3)player->getWorldPosition();
}

// Rotation plane is updated and lever interpolates toward target angle.
void InteractableLeverComponent::update()
{
	// Recalculate rotation plane basis each frame for movable levers
	if (is_movable)
	{
		valve_plane_normal = (transformation_node->getWorldRotation() * rotation_axis).normalize();
		valve_plane_x = (quat(valve_plane_normal, -rotation_angle) * (vec3)transformation_node->getWorldDirection(AXIS_Z)).normalize();
		valve_plane_y = cross(valve_plane_normal, valve_plane_x).normalize();
	}

	// Project player view onto rotation plane when interaction is active
	if (used_current_frame)
	{
		lever_position = (vec3)transformation_node->getWorldPosition();
		// Extend player view ray to lever distance and compute direction
		player_vec = (player_position + player_view_direction * (player_position - lever_position).length()) - lever_position;

		// Project onto rotation plane to get target direction
		rotation_vec = valve_plane_x * dot(valve_plane_x, player_vec) + valve_plane_y * dot(valve_plane_y, player_vec);
		rotation_vec.normalize();
	}

	// Smoothly interpolate toward target angle while interacting or settling
	if (Math::abs(rotation_angle - rotation_angle_target) > angle_eps || used_current_frame)
	{
		// Compute angle between plane X-axis and rotation vector
		rotation_angle_target = getAngle(valve_plane_x, rotation_vec, vec3(valve_plane_normal));
		rotation_angle_target = clamp(rotation_angle_target, 0.0f, rotation_angle_range);
		// Linear interpolation provides smooth movement
		rotation_angle = lerp(rotation_angle, rotation_angle_target, 0.2f);
		transformation_node->setRotation(initial_rotation * quat(rotation_axis, rotation_angle));
	}

	// Reset frame flags after processing
	if (used_current_frame)
	{
		rotation_vec_old = rotation_vec;
		used_last_frame = used_current_frame;
		used_current_frame = false;
	}
}

// Normalized lever position is returned (0 = start, 1 = full range).
float InteractableLeverComponent::getValue()
{
	return rotation_angle / rotation_angle_range;
}