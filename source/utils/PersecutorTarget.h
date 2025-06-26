#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>

class PersecutorTarget : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PersecutorTarget, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, target);
	PROP_PARAM(Toggle, wait_angle, true);
	PROP_PARAM(Float, wait_angle_time, 5.0f, "", "", "", "wait_angle=1");
	PROP_PARAM(Toggle, wheel_distance, true);
	PROP_PARAM(Vec2, wheel_distance_clamp, Unigine::Math::vec2(0.0f, 100.0f), "", "", "", "wheel_distance=1");
	PROP_PARAM(Float, wheel_distance_speed, 50.0f, "", "", "", "wheel_distance=1");
	PROP_PARAM(Toggle, lerp_pos, false);
	PROP_PARAM(Float, lerp_pos_k, 5.0f, "", "", "", "lerp_pos=1");

private:
	void init();
	void update();
	void shutdown();

	Unigine::PlayerPersecutorPtr player;
	bool valid = false;
	bool fixed = false;
	Unigine::NodePtr target_node;
	float timer = 0.0f;
	float distance_spread = 0.0f;
};