// Button control element with toggle and momentary modes. Supports animated transitions
// between on/off states with configurable duration. When togglable, each press flips
// the state; otherwise the button returns to default when released.

#include "InteractableButtonComponent.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(InteractableButtonComponent);

using namespace Unigine;
using namespace Math;

// On/off positions and rotations are computed from initial transform.
void InteractableButtonComponent::init()
{
	// Default to component node if no explicit transformation target
	if (!transformation_node)
	{
		transformation_node = node;
	}

	button_state = default_state;
	rotation_axis = normalize(rotation_axis.get());
	// Capture current rotation as the "off" state
	rotation_off_quat = transformation_node->getRotation();
	rotation_on_quat = rotation_off_quat * quat(rotation_axis.get(), rotation_angle);

	// Capture current position as the "off" state
	position_off = transformation_node->getPosition();
	position_on = transformation_node->getPosition() + (Vec3)position;

	// Start animation at end if default state is "on"
	if (default_state)
	{
		time = anim_duration;
	}
}

// Animation time is advanced and transform is interpolated toward target state.
void InteractableButtonComponent::update()
{
	// Momentary buttons return to default when released
	if (!togglable && !used_current_frame)
	{
		button_state = default_state;
	}

	// Advance animation time toward target based on button state
	time = clamp((time - (Game::getIFps() * (button_state ? -1.0f : 1.0f))), 0.0f, anim_duration);

	// Calculate interpolation value for smooth transitions
	if (has_animation_time)
	{
		value = (time / anim_duration.get());
	}
	else
	{
		value = button_state ? 1.0f : 0.0f;
	}

	// Interpolate position and rotation between off and on states
	Vec3 new_position = static_cast<Vec3>(lerp(position_off, position_on, value));
	quat new_rotation = slerp(rotation_off_quat, rotation_on_quat, value);

	transformation_node->setTransform(
		static_cast<Mat4>(rotate(new_rotation)) * translate(new_position));

	used_last_frame = used_current_frame;
	used_current_frame = false;
}

// Button state is toggled or set based on interaction mode.
void InteractableButtonComponent::use(PlayerPtr player, Unigine::Math::vec3 intersection_point)
{
	used_current_frame = true;
	if (togglable)
	{
		// Toggle mode: flip state only on initial press
		if (!used_last_frame)
		{
			button_state = 1 - button_state;
		}
	}
	else
	{
		// Momentary mode: set to opposite of default while held
		if (used_last_frame)
		{
			button_state = 1 - default_state;
		}
	}
}

// Animated value or discrete state is returned depending on configuration.
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