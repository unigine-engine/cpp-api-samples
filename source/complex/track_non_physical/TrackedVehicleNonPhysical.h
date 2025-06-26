#pragma once
#include <UnigineComponentSystem.h>
#include <UniginePhysics.h>

using namespace Unigine;
using namespace Math;

class InputManager;
class NonPhysicalTracks;

class TrackedVehicleNonPhysical : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackedVehicleNonPhysical, Unigine::ComponentBase)
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
	PROP_PARAM(Float, wheel_mass, 0.5f)
	PROP_PARAM(Float, wheel_linear_spring, 27)
	PROP_PARAM(Float, wheel_linear_damping, 10)
	PROP_PARAM(Float, wheel_linear_limit_from, -0.5f)
	PROP_PARAM(Float, wheel_linear_limit_to, 0.3f)
	PROP_PARAM(Float, wheel_tangent_friction, 7)
	PROP_PARAM(Float, wheel_tangent_angle, 0.05f)
	PROP_PARAM(Float, wheel_binormal_friction, 7)
	PROP_PARAM(Float, wheel_binormal_angle, 0.05f)
	PROP_PARAM(Float, wheel_linear_distance, 0.2f)
	PROP_PARAM(Int, wheel_num_iterations, 4)
	PROP_PARAM(Float, track_height, 0.05f)
	PROP_ARRAY(Node, wheels_left)
	PROP_ARRAY(Node, wheels_right)

private:
	Unigine::BodyRigidPtr body;
	Unigine::Vector<Unigine::NodePtr> wheels_left_physics;
	Unigine::Vector<Unigine::NodePtr> wheels_right_physics;
	Unigine::Vector<Unigine::JointWheelPtr> wheels_left_joint;
	Unigine::Vector<Unigine::JointWheelPtr> wheels_right_joint;
	Unigine::Vector<Unigine::Math::Vec3> wheels_left_pos;
	Unigine::Vector<Unigine::Math::Vec3> wheels_right_pos;
	
	float throttle = 0, turn = 0, turn_smooth = 0;
	float wheel_angular_speed = 0;
	float wheel_max_angular_speed = 0;
	float wheel_max_back_angular_speed = 0;
	
	float current_wheels_angular_damping = 0;
	
	Unigine::Math::Vec3 last_pos;
	Unigine::Vector<NonPhysicalTracks *> tracks;
	
	InputManager *input = nullptr;

private:
	void init();
	void update();
	void updatePhysics();
	void shutdown();
	void configure_wheel(const Unigine::NodePtr &wheel_node,
		Unigine::Vector<Unigine::NodePtr> &physics_vector,
		Unigine::Vector<Unigine::JointWheelPtr> &joints_vector);
};
