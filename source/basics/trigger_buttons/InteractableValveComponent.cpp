#include "InteractableValveComponent.h"

#include "UnigineVisualizer.h"
REGISTER_COMPONENT(InteractableValveComponent);

using namespace Unigine;
using namespace Math;

const float angle_eps = 0.05f;

void InteractableValveComponent::init()
{
	if (!transformation_node)
	{
		transformation_node = node;
	}

	valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_X).normalize();
	valve_plane_y = (vec3)transformation_node->getWorldDirection(AXIS_Y).normalize();
	valve_plane_normal = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();

	player_view_direction = valve_plane_y;
	grab_point = (vec3)transformation_node->getWorldPosition();
}

void InteractableValveComponent::use(Unigine::PlayerPtr player,
	Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	player_view_direction = player->getViewDirection();
	player_position = (vec3)player->getWorldPosition();

	grab_point = intersection_point;
	if (!used_last_frame)
	{
		vec3 grab_vec = (grab_point - (vec3)transformation_node->getWorldPosition());
		grab_vec_old = valve_plane_x * dot(valve_plane_x, grab_vec)
			+ valve_plane_y * dot(valve_plane_y, grab_vec);
	}
}

void InteractableValveComponent::update()
{
	if (is_movable)
	{
		valve_plane_x = (vec3)transformation_node->getWorldDirection(AXIS_X).normalize();
		valve_plane_y = (vec3)transformation_node->getWorldDirection(AXIS_Y).normalize();
		valve_plane_normal = (vec3)transformation_node->getWorldDirection(AXIS_Z).normalize();
	}
	if (used_current_frame)
	{
		vec3 valve_position = (vec3)transformation_node->getWorldPosition();
		grab_vec = (player_position
					   + player_view_direction * (player_position - valve_position).length())
			- valve_position;

		rotation_vec = valve_plane_x * dot(valve_plane_x, grab_vec)
			+ valve_plane_y * dot(valve_plane_y, grab_vec);
		rotation_vec.normalize();
	}
	if (Math::abs(rotation_angle - rotation_angle_target) > angle_eps || used_current_frame)
	{
		rotation_angle_target += getAngle(vec3(grab_vec_old), vec3(rotation_vec),
			vec3(valve_plane_normal));
		rotation_angle_target = clamp(rotation_angle_target, 0.0f, rotation_angle_range);
		rotation_angle = lerp(rotation_angle, rotation_angle_target, 0.1f);

		transformation_node->setRotation(
			quat(vec3(transformation_node->getDirection(AXIS_Z)), rotation_angle));
	}
	grab_vec_old = rotation_vec;
	used_last_frame = used_current_frame;
	used_current_frame = false;
}

float InteractableValveComponent::getValue()
{
	return rotation_angle / rotation_angle_range;
}
