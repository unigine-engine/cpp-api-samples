#include "TrackedVehiclePhysical.h"

#include <UnigineGame.h>
#include <UnigineConsole.h>

#include "../../utils/InputManager.h"

REGISTER_COMPONENT(TrackedVehiclePhysical);

using namespace Unigine;
using namespace Math;

const float MaxWheelAngularDamping = 380.f;
const float MinWheelAngularDamping = 1.f;
const float WheelAngularDampingIncreaseSpeed = 80.f;
const float WheelAngularDampingDecreaseSpeed = 320.f;
const float MaxWheelTorque = 5;


void TrackedVehiclePhysical::init()
{
	if (input_manager_node.nullCheck())
		Log::error("TrackedVehiclePhysical::init(): Input Manager Node is not set\n");

	input = ComponentSystem::get()->getComponentInChildren<InputManager>(input_manager_node);
	if (!input)
		Log::error("TrackedVehiclePhysical::init(): Input Manager Node does not contain InputManager\n");

	// disable collision/intersection flags
	std::function<void(const NodePtr &)> disable_intersections = [&, this](const NodePtr &n) {
		if (!n)
			return;
		
		if (n->isObject())
		{
			ObjectPtr obj = static_ptr_cast<Object>(n);
			for (int i = 0; i < obj->getNumSurfaces(); i++)
			{
				obj->setIntersection(false, i);
				obj->setCollision(false, i);
				obj->setPhysicsIntersection(false, i);
			}
		}
		
		if (n->getType() == Node::NODE_REFERENCE)
			disable_intersections(static_ptr_cast<NodeReference>(n)->getReference());
		
		for (int i = 0; i < n->getNumChildren(); i++)
			disable_intersections(n->getChild(i));
	};
	disable_intersections(node);

	// convert values
	acceleration = acceleration * Consts::PI;
	deceleration = deceleration * Consts::PI;
	wheel_max_angular_speed = (max_speed / 3.6f) * Consts::PI;
	wheel_max_back_angular_speed = (max_back_speed / 3.6f) * Consts::PI;

	// set up physics
	body = node->getObjectBodyRigid();
	body->setFrozenLinearVelocity(0.3f);
	body->setAngularDamping(angular_damping);
	body->setLinearDamping(1.f);

	for (int i = 0; i < wheels_left.size(); i++)
		configure_wheel(wheels_left[i], wheels_left_joint);

	for (int i = 0; i < wheels_right.size(); i++)
		configure_wheel(wheels_right[i], wheels_right_joint);
}

void TrackedVehiclePhysical::update()
{
	if (!input)
		return;

	float ifps = Game::getIFps();
	
	throttle = 0;
	turn = 0;

	if (!Console::isActive())
	{
		throttle = input->getThrottle();
		turn = input->getTurn();
	}

	// speed calculations
	wheel_angular_speed += throttle * ifps * acceleration;

	if (throttle == 0 || Math::sign(throttle) != Math::sign(wheel_angular_speed))
	{
		wheel_angular_speed = -wheel_angular_speed <= ifps * deceleration ? 0
			: wheel_angular_speed - Math::sign(wheel_angular_speed) * ifps * deceleration;
		if (turn == 0)
			current_wheels_angular_damping += ifps * WheelAngularDampingIncreaseSpeed;
		else
			current_wheels_angular_damping = MinWheelAngularDamping;
	}
	else
		current_wheels_angular_damping -= ifps * WheelAngularDampingDecreaseSpeed;

	wheel_angular_speed = clamp(wheel_angular_speed, -wheel_max_back_angular_speed, wheel_max_angular_speed);
	turn_smooth = lerp(turn_smooth, turn * Math::sign(wheel_angular_speed), turn_smooth_factor * ifps);
	current_wheels_angular_damping = clamp(current_wheels_angular_damping, MinWheelAngularDamping, MaxWheelAngularDamping);
}

void TrackedVehiclePhysical::updatePhysics()
{
	float torque = clamp(float(fabs(wheel_angular_speed)) * 0.3f, 0.f, MaxWheelTorque);

	// move forward/backward
	for (int i = 0; i < wheels_left_joint.size(); i++)
	{
		wheels_left_joint[i]->setAngularDamping(current_wheels_angular_damping);
		wheels_left_joint[i]->setAngularVelocity(wheel_angular_speed);
		wheels_left_joint[i]->setAngularTorque(torque);
	}

	for (int i = 0; i < wheels_right_joint.size(); i++)
	{
		wheels_right_joint[i]->setAngularDamping(current_wheels_angular_damping);
		wheels_right_joint[i]->setAngularVelocity(wheel_angular_speed);
		wheels_right_joint[i]->setAngularTorque(torque);
	}

	// turn left/right
	vec3 ang_vel = body->getAngularVelocity();
	body->setAngularVelocity(vec3(ang_vel.x, ang_vel.y, -turn_speed * turn_smooth * Physics::getIFps()));
}

void TrackedVehiclePhysical::shutdown()
{
	for (int i = 0; i < wheels_left_joint.size(); i++)
		wheels_left_joint[i].deleteLater();
	wheels_left_joint.clear();

	for (int i = 0; i < wheels_right_joint.size(); i++)
		wheels_right_joint[i].deleteLater();
	wheels_right_joint.clear();
}

void TrackedVehiclePhysical::configure_wheel(const NodePtr &wheel_node, Vector<JointHingePtr> &joints_vector)
{
	JointHingePtr joint = checked_ptr_cast<JointHinge>(wheel_node->getObjectBodyRigid()->getJoint(0));

	joint->setNumIterations(wheel_num_iterations);
	joint->setAngularTorque(MaxWheelTorque);
	joint->setAngularVelocity(0);
	joint->setAngularSpring(wheel_spring);

	joint->setLinearRestitution(0.4f);
	joint->setAngularRestitution(0.1f);

	joints_vector.append(joint);
}
