#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMathLib.h>
#include <UnigineNodes.h>
#include <UniginePlayers.h>
#include <UnigineVector.h>

class SimplePathControl : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimplePathControl, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, wait_before_start, 0);
	PROP_PARAM(Float, speed, 13);
	PROP_PARAM(Float, torque_param, 1);
	PROP_PARAM(Node, path_node);
	PROP_PARAM(Toggle, circle, false);
	PROP_PARAM(Toggle, init_button, false);
	PROP_PARAM(Toggle, enable_button, false);

	PROP_PARAM(Float, eps_d, 0.1);

private:
	void init();
	void update();
	void shutdown();

	Unigine::Vector<Unigine::Math::Mat4> path;
	Unigine::Math::Mat4 prev_path;
	int current_path_index = 0;
	int dir = 1;
	float rot_acceleration = 0.0f;
};
