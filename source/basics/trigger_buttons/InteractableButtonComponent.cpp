#include "InteractableButtonComponent.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(InteractableButtonComponent);

using namespace Unigine;
using namespace Math;

void InteractableButtonComponent::init()
{
	if (!transformation_node)
	{
		transformation_node = node;
	}

	button_state = default_state;
	rotation_axis = normalize(rotation_axis.get());
	rotation_off_quat = transformation_node->getRotation();
	rotation_on_quat = rotation_off_quat * quat(rotation_axis.get(), rotation_angle);

	position_off = transformation_node->getPosition();
	position_on = transformation_node->getPosition() + (Vec3)position;

	if (default_state)
	{
		time = anim_duration;
	}
}

void InteractableButtonComponent::update()
{
	if (!togglable && !used_current_frame)
	{
		button_state = default_state;
	}

	time = clamp((time - (Game::getIFps() * (button_state ? -1.0f : 1.0f))), 0.0f, anim_duration);

	if (has_animation_time)
	{
		value = (time / anim_duration.get());
	}
	else
	{
		value = button_state ? 1.0f : 0.0f;
	}

	Vec3 new_position = static_cast<Vec3>(lerp(position_off, position_on, value));
	quat new_rotation = slerp(rotation_off_quat, rotation_on_quat, value);

	transformation_node->setTransform(
		static_cast<Mat4>(rotate(new_rotation)) * translate(new_position));

	used_last_frame = used_current_frame;
	used_current_frame = false;
}

void InteractableButtonComponent::use(PlayerPtr player, Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	if (togglable)
	{
		if (!used_last_frame)
		{
			button_state = 1 - button_state;
		}
	}
	else
	{
		if (used_last_frame)
		{
			button_state = 1 - default_state;
		}
	}
}

float InteractableButtonComponent::getValue()
{
	if (has_animation_time)
	{
		return value;
	}
	else
	{
		return button_state ? 1.0f : 0.0f;
	}
}