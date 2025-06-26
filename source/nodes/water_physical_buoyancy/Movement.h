#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>
class Movement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Movement, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);
	PROP_PARAM(Node, movement_point_node);

private:
	void init();
	void update();
	void update_physics();

private:
	Unigine::BodyRigidPtr body_rigid;
	Unigine::Math::Vec3 direction;
	float force_multiplier = 5.0f;
};
