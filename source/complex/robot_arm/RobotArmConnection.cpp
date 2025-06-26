#include "RobotArmConnection.h"

#include <UniginePhysicals.h>

REGISTER_COMPONENT(RobotArmConnection);

using namespace Unigine;
using namespace Math;

void RobotArmConnection::init()
{
	auto trigger = checked_ptr_cast<PhysicalTrigger>(connection_trigger.get());

	trigger->getEventEnter().connect(this, &RobotArmConnection::on_trigger_enter);
	trigger->getEventLeave().connect(this, &RobotArmConnection::on_trigger_leave);

	int num = node->getObjectBody()->findJoint("connection_joint");
	if (num != -1)
	{
		joint_fixed = checked_ptr_cast<JointFixed>(node->getObjectBody()->getJoint(num));
	}
}

void RobotArmConnection::update()
{
	if (Input::isKeyDown(Input::KEY_C) && connection_candidate != nullptr)
	{
		connected = true;

		joint_fixed->setBody1(connection_candidate);

		auto itransform = inverse(connection_candidate->getTransform());
		auto anchor_1_transform = itransform * connection_point.get()->getWorldTransform();
		joint_fixed->setAnchor1(anchor_1_transform.getTranslate());
		joint_fixed->setRotation1(anchor_1_transform.getRotate().getMat3());

		joint_fixed->setEnabled(true);
	}

	if (Input::isKeyDown(Input::KEY_V))
	{
		connected = false;
		joint_fixed->setEnabled(false);
	}
}

void RobotArmConnection::on_trigger_enter(const BodyPtr &body)
{
	if (!connection_candidate)
		connection_candidate = checked_ptr_cast<BodyRigid>(body);
}

void RobotArmConnection::on_trigger_leave(const BodyPtr& body)
{
	if (connection_candidate == checked_ptr_cast<BodyRigid>(body))
	{
		connection_candidate = nullptr;
		connected = false;
		joint_fixed->setEnabled(false);
	}
}
