#pragma once

#include <UnigineComponentSystem.h>


class RobotArmController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RobotArmController, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	struct ArmJoint : public Unigine::ComponentStruct
	{
		PROP_PARAM(Node, arm_node, nullptr);
		PROP_PARAM(Float, speed, 100.0f);

		void init();
		void update(float ifps, Unigine::Input::KEY positiveAxis, Unigine::Input::KEY negativeAxis);

	private:
		Unigine::JointHingePtr arm_joint_hinge = nullptr;

		void rotate(float angle);
	};

	PROP_STRUCT(ArmJoint, arm_joint_0);
	PROP_STRUCT(ArmJoint, arm_joint_1);
	PROP_STRUCT(ArmJoint, arm_joint_2);
	PROP_STRUCT(ArmJoint, arm_joint_3);
	PROP_STRUCT(ArmJoint, arm_joint_4);
	PROP_STRUCT(ArmJoint, arm_joint_5);

private:
	void init();
	void update();
};
