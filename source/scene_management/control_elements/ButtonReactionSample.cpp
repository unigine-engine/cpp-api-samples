// Demonstrates reaction to interactable control state. The node's transform is
// interpolated between initial and final positions/rotations based on the
// interactable's normalized value (0-1).

#include "ButtonReactionSample.h"

REGISTER_COMPONENT(ButtonReactionSample);

using namespace Unigine;
using namespace Math;

// Initial transform is captured and interactable reference is resolved.
void ButtonReactionSample::init()
{
	// Look up interactable interface on the linked node
	interactable_comp = ComponentSystem::get()->getComponent<Interactable>(interatable_node);

	// Capture starting transform for interpolation baseline
	initial_position = node->getPosition();
	initial_rotation = node->getRotation();

	// Compute target rotation by applying configured euler offsets
	final_rotation = initial_rotation * quat(rotation.get().x, rotation.get().y, rotation.get().z);
}

// Node transform is interpolated based on current interactable value.
void ButtonReactionSample::update()
{
	if (!interactable_comp) return;

	// Read normalized state from the linked interactable
	float value = interactable_comp->getValue();

	// Interpolate rotation and position based on interactable value
	quat new_rotation = slerp(initial_rotation, final_rotation, value);
	Vec3 new_position = lerp(initial_position, initial_position + (Vec3)position, value);

	node->setTransform(translate(new_position) * (Mat4)rotate(new_rotation));
}
