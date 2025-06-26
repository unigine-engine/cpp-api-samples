#pragma once

#include <UnigineComponentSystem.h>

class RobotArmConnection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RobotArmConnection, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, connection_point, nullptr);
	PROP_PARAM(Node, connection_trigger, nullptr);

private:
	void init();
	void update();
	void on_trigger_enter(const Unigine::BodyPtr& body);
	void on_trigger_leave(const Unigine::BodyPtr& body);

	Unigine::JointFixedPtr joint_fixed = nullptr;
	Unigine::BodyRigidPtr connection_candidate = nullptr;
	bool connected = false;
};
