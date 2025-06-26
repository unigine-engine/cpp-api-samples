#include "ButtonReactionSample.h"

REGISTER_COMPONENT(ButtonReactionSample);

using namespace Unigine;
using namespace Math;

void ButtonReactionSample::init()
{
	interactable_comp = ComponentSystem::get()->getComponent<Interactable>(interatable_node);

	initial_position = node->getPosition();
	initial_rotation = node->getRotation();

	final_rotation = initial_rotation * quat(rotation.get().x, rotation.get().y, rotation.get().z);
}

void ButtonReactionSample::update()
{
	if (!interactable_comp) return;

	float value = interactable_comp->getValue();

	quat new_rotation = slerp(initial_rotation, final_rotation, value);
	Vec3 new_position = lerp(initial_position, initial_position + (Vec3)position, value);

	node->setTransform(translate(new_position) * (Mat4)rotate(new_rotation));
}
