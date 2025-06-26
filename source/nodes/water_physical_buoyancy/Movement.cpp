#include "Movement.h"

REGISTER_COMPONENT(Movement);

using namespace Unigine;

void Movement::init()
{
	body_rigid = node->getObjectBodyRigid();
	if (!body_rigid)
	{
		Log::message("Movement::init: couldn't get RigidBody");
	}
}

void Movement::update()
{
	direction = static_cast<Math::Vec3>(Math::vec3_zero);

	if (Input::isKeyPressed(Input::KEY::KEY_W))
	{
		direction += static_cast<Math::Vec3>(Math::vec3_forward);
	}

	if (Input::isKeyPressed(Input::KEY::KEY_S))
	{
		direction += static_cast<Math::Vec3>(Math::vec3_back);
	}

	if (Input::isKeyPressed(Input::KEY::KEY_A))
	{
		direction += static_cast<Math::Vec3>(Math::vec3_right * 0.15f);
	}

	if (Input::isKeyPressed(Input::KEY::KEY_D))
	{
		direction += static_cast<Math::Vec3>(Math::vec3_left * 0.15f);
	}
}

void Movement::update_physics()
{
	direction = body_rigid->getTransform().getRotate() * direction;
	body_rigid->addWorldForce(movement_point_node->getWorldPosition(),
		static_cast<Math::vec3>(direction) * body_rigid->getMass() * force_multiplier);
}
