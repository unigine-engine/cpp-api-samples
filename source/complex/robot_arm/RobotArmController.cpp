#include "RobotArmController.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(RobotArmController);

using namespace Unigine;

void RobotArmController::ArmJoint::init()
{
	arm_joint_hinge = checked_ptr_cast<JointHinge>(arm_node.get()->getObjectBody()->getJoint(0));
}

void RobotArmController::ArmJoint::update(float ifps, Unigine::Input::KEY positiveAxis, Unigine::Input::KEY negativeAxis)
{
	if (Input::isKeyPressed(positiveAxis))
		rotate(speed.get() * ifps);

	if (Input::isKeyPressed(negativeAxis))
		rotate(-speed.get() * ifps);
}

void RobotArmController::ArmJoint::rotate(float angle)
{
	if (arm_joint_hinge)
	{
		float currentAngle = arm_joint_hinge->getAngularAngle();
		currentAngle += angle;
		if (currentAngle < -180)
			currentAngle += 360.0f;
		if (currentAngle > 180)
			currentAngle -= 360.0f;

		if (currentAngle < arm_joint_hinge->getAngularLimitFrom())
			currentAngle = arm_joint_hinge->getAngularLimitFrom();

		if (currentAngle > arm_joint_hinge->getAngularLimitTo())
			currentAngle = arm_joint_hinge->getAngularLimitTo();

		arm_joint_hinge->setAngularAngle(currentAngle);
	}
}

void RobotArmController::init()
{
	arm_joint_0.get().init();
	arm_joint_1.get().init();
	arm_joint_2.get().init();
	arm_joint_3.get().init();
	arm_joint_4.get().init();
	arm_joint_5.get().init();
}

void RobotArmController::update()
{
	arm_joint_0.get().update(Game::getIFps(), Input::KEY_H, Input::KEY_F);
	arm_joint_1.get().update(Game::getIFps(), Input::KEY_T, Input::KEY_G);
	arm_joint_2.get().update(Game::getIFps(), Input::KEY_I, Input::KEY_K);
	arm_joint_3.get().update(Game::getIFps(), Input::KEY_J, Input::KEY_L);
	arm_joint_4.get().update(Game::getIFps(), Input::KEY_U, Input::KEY_O);
	arm_joint_5.get().update(Game::getIFps(), Input::KEY_R, Input::KEY_Y);
}
