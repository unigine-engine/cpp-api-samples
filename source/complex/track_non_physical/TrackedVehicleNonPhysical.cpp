#include "TrackedVehicleNonPhysical.h"

#include <UnigineGame.h>
#include <UnigineConsole.h>

#include "../../utils/InputManager.h"
#include "NonPhysicalTracks.h"

REGISTER_COMPONENT(TrackedVehicleNonPhysical);

using namespace Unigine;
using namespace Math;

const float MaxWheelAngularDamping = 380.f;
const float MinWheelAngularDamping = 1.f;
const float WheelAngularDampingIncreaseSpeed = 80.f;
const float WheelAngularDampingDecreaseSpeed = 320.f;
const float MaxWheelTorque = 4;


void TrackedVehicleNonPhysical::init()
{
	if (input_manager_node.nullCheck())
		Log::error("TrackedVehicleNonPhysical::init(): Input Manager Node is not set\n");

	input = ComponentSystem::get()->getComponentInChildren<InputManager>(input_manager_node);
	if (!input)
		Log::error("TrackedVehicleNonPhysical::init(): Input Manager Node does not contain InputManager component\n");

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

	wheels_left_pos.resize(wheels_left.size());
	for (int i = 0; i < wheels_left.size(); i++)
	{
		configure_wheel(wheels_left[i], wheels_left_physics, wheels_left_joint);
		wheels_left_pos[i] = wheels_left[i]->getPosition();
	}
	wheels_right_pos.resize(wheels_right.size());
	for (int i = 0; i < wheels_right.size(); i++)
	{
		configure_wheel(wheels_right[i], wheels_right_physics, wheels_right_joint);
		wheels_right_pos[i] = wheels_right[i]->getPosition();
	}

	// find all tracks
	getComponentsInChildren<NonPhysicalTracks>(node, tracks);
	last_pos = node->getWorldPosition();
}

void TrackedVehicleNonPhysical::update()
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

	// update wheels
	Vec3 new_pos = node->getWorldPosition();
	Vec3 offset = node->getIWorldTransform() * last_pos;
	float offset_left = static_cast<float>(offset.y - turn_smooth * ifps * 2.0f);
	float offset_right = static_cast<float>(offset.y + turn_smooth * ifps * 2.0f);

	for (int i = 0; i < wheels_left.size(); i++)
	{
		wheels_left[i]->setRotation(wheels_left[i]->getRotation()
			* quat(0, -360.0f * offset_left / (2.0f * Consts::PI * wheels_left_joint[i]->getWheelRadius()), 0));
		wheels_left_pos[i].z = wheels_left_physics[i]->getPosition().z;
		wheels_left[i]->setPosition(wheels_left_pos[i]);
	}

	for (int i = 0; i < wheels_right.size(); i++)
	{
		wheels_right[i]->setRotation(wheels_right[i]->getRotation()
			* quat(0, -360.0f * offset_right / (2.0f * Consts::PI * wheels_right_joint[i]->getWheelRadius()), 0));
		wheels_right_pos[i].z = wheels_right_physics[i]->getPosition().z;
		wheels_right[i]->setPosition(wheels_right_pos[i]);
	}
	
	last_pos = new_pos;

	// update tracks
	for (int i = 0; i < tracks.size(); i++)
		tracks[i]->setOffset(tracks[i]->side.get() == 0 /*left*/ ? -offset_left : -offset_right);
}

void TrackedVehicleNonPhysical::updatePhysics()
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

void TrackedVehicleNonPhysical::shutdown()
{
	for (int i = 0; i < wheels_left_joint.size(); i++)
	{
		wheels_left_joint[i].deleteLater();
		wheels_left_physics[i].deleteLater();
	}
	wheels_left_joint.clear();
	wheels_left_physics.clear();

	for (int i = 0; i < wheels_right_joint.size(); i++)
	{
		wheels_right_joint[i].deleteLater();
		wheels_right_physics[i].deleteLater();
	}
	wheels_right_joint.clear();
	wheels_right_physics.clear();
}

void TrackedVehicleNonPhysical::configure_wheel(const NodePtr &wheel_node,
	Vector<NodePtr> &physics_vector, Vector<JointWheelPtr> &joints_vector)
{
	float radius = (wheel_node->getBoundBox().maximum - wheel_node->getBoundBox().minimum).z / 2.0f;

	ObjectDummyPtr dummy = ObjectDummy::create();
	dummy->setParent(wheel_node->getParent());
	dummy->setWorldTransform(wheel_node->getWorldTransform());
	
	JointWheelPtr joint = JointWheel::create(body, BodyRigid::create(dummy));
	joint->setWorldAxis1(wheel_node->getWorldDirection(Math::AXIS_NY));

	joint->setWheelMass(wheel_mass);
	joint->setLinearSpring(wheel_linear_spring);
	joint->setLinearDamping(wheel_linear_damping);
	joint->setWheelRadius(radius + track_height);
	joint->setTangentFriction(wheel_tangent_friction);
	joint->setTangentAngle(wheel_tangent_angle);
	joint->setBinormalFriction(wheel_binormal_friction);
	joint->setBinormalAngle(wheel_binormal_angle);
	joint->setNumIterations(wheel_num_iterations);

	joint->setAngularTorque(MaxWheelTorque);
	joint->setAngularVelocity(0);

	joint->setLinearRestitution(0.4f);
	joint->setAngularRestitution(0.1f);

	joint->setLinearLimitFrom(wheel_linear_limit_from);
	joint->setLinearLimitTo(wheel_linear_limit_to);
	joint->setLinearDistance(wheel_linear_distance);

	physics_vector.append(dummy);
	joints_vector.append(joint);
}
