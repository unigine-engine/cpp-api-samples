#pragma once
#include "UpdatePhysicsUsageController.h"

#include <Unigine.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(UpdatePhysicsUsageController);

void UpdatePhysicsUsageController::init()
{
	rigid_body = node->getObjectBodyRigid();

	if (!rigid_body)
	{
		Log::error("PhysicsIFpsController::init can't find rigid body on the node!\n");
	}
	current_force = linear_force;

	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);
}

void UpdatePhysicsUsageController::update()
{
	//visualizing current linear velocity
	Visualizer::renderVector(rigid_body->getPosition(), rigid_body->getPosition() + Vec3(rigid_body->getLinearVelocity()), vec4_red,0.5f);

	//NOTICE that methods: update and udpate_physics registered in different component Macros and code is the same for both usage examples
	// using update() to move node with physics
	if (use_update) 
	{
		movement();
	}
}

void UpdatePhysicsUsageController::movement()
{
	rigid_body->addForce(vec3_right * current_force);

	if (node->getWorldPosition().x > 5)
		current_force = -linear_force;
	if (node->getWorldPosition().x < -5)
		current_force = linear_force;
}

void UpdatePhysicsUsageController::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);
}

void UpdatePhysicsUsageController::update_physics()
{
	//NOTICE that methods: update and udpate_physics registered in different component Macros and code is the same for both usage examples
	// using update() to move node with physics
	if (!use_update)
	{
		movement();
	}
}
