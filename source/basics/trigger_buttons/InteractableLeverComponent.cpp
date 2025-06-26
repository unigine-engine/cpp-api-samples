#include "InteractableLeverComponent.h"
#include "UnigineVisualizer.h"
REGISTER_COMPONENT(InteractableLeverComponent);

using namespace Unigine;
using namespace Math;


const float angle_eps = 0.05f;

void InteractableLeverComponent::init()
{
	if (!transformation_node)
		transformation_node = node;

	rotation_axis = normalize(rotation_axis.get());
	valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();
	valve_plane_normal = (transformation_node->getWorldRotation() * rotation_axis).normalize();
	valve_plane_y = cross(valve_plane_normal, valve_plane_x).normalize();
	initial_rotation = transformation_node->getWorldRotation();
}

void InteractableLeverComponent::use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	player_view_direction = player->getViewDirection();
	player_position = (vec3)player->getWorldPosition();
}

void InteractableLeverComponent::update()
{
	if (is_movable)
	{
		valve_plane_normal = (transformation_node->getWorldRotation() * rotation_axis).normalize();
		valve_plane_x = (quat(valve_plane_normal, -rotation_angle) * (vec3)transformation_node->getWorldDirection(AXIS_Z)).normalize();
		valve_plane_y = cross(valve_plane_normal, valve_plane_x).normalize();
	}

	if (used_current_frame)
	{
		lever_position = (vec3)transformation_node->getWorldPosition();
		player_vec = (player_position + player_view_direction * (player_position - lever_position).length()) - lever_position;

		rotation_vec = valve_plane_x * dot(valve_plane_x, player_vec) + valve_plane_y * dot(valve_plane_y, player_vec);
		rotation_vec.normalize();
	}

	if (Math::abs(rotation_angle - rotation_angle_target) > angle_eps || used_current_frame)
	{
		rotation_angle_target = getAngle(valve_plane_x, rotation_vec, vec3(valve_plane_normal));
		rotation_angle_target = clamp(rotation_angle_target, 0.0f, rotation_angle_range);
		rotation_angle = lerp(rotation_angle, rotation_angle_target, 0.2f);
		transformation_node->setRotation(initial_rotation * quat(rotation_axis, rotation_angle));
	}

	if (used_current_frame)
	{
		rotation_vec_old = rotation_vec;
		used_last_frame = used_current_frame;
		used_current_frame = false;
	}
}

float InteractableLeverComponent::getValue()
{
	return rotation_angle / rotation_angle_range;
}