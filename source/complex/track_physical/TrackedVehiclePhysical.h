#pragma once
#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>

using namespace Unigine;
using namespace Math;

class InputManager;

class TrackedVehiclePhysical : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackedVehiclePhysical, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_UPDATE_PHYSICS(updatePhysics)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, input_manager_node)

	// parameters: controls
	PROP_PARAM(Float, acceleration, 5)
	PROP_PARAM(Float, deceleration, 10)
	PROP_PARAM(Float, max_speed, 50)      // km/h
	PROP_PARAM(Float, max_back_speed, 15) // km/h
	PROP_PARAM(Float, turn_speed, 45)     // deg/s
	PROP_PARAM(Float, turn_smooth_factor, 5.0f)
	PROP_PARAM(Float, angular_damping, 2.0f)

	// parameters: wheels
	PROP_PARAM(Float, wheel_spring, 0)
	PROP_PARAM(Int, wheel_num_iterations, 4)
	PROP_ARRAY(Node, wheels_left)
	PROP_ARRAY(Node, wheels_right)

private:
	Unigine::BodyRigidPtr body;
	Unigine::Vector<Unigine::JointHingePtr> wheels_left_joint;
	Unigine::Vector<Unigine::JointHingePtr> wheels_right_joint;

	float throttle = 0.f, turn = 0.f, turn_smooth = 0.f;
	float wheel_angular_speed = 0.f;
	float wheel_max_angular_speed = 0.f;
	float wheel_max_back_angular_speed = 0.f;

	float current_wheels_angular_damping = 0.f;

	InputManager * input = nullptr;

private:
	void init();
	void update();
	void updatePhysics();
	void shutdown();
	void configure_wheel(const Unigine::NodePtr &wheel_node,
		Unigine::Vector<Unigine::JointHingePtr> &joints_vector);
};
