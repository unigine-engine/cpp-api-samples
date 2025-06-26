#pragma once
#include "UnigineComponentSystem.h"

class ZoomController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ZoomController, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	void focus_on_target(Unigine::NodePtr target);
	void udpate_zoom_factor(float zoom_factor);
	void reset();
private:

	float default_FOV = 60.0f;
	float default_distance_scale = 1.0f; //enables object rendering in far places
	float default_sensivity = 1.0f;
	float default_player_turning = 90.0f;

	Unigine::PlayerPtr player;

	void init();
	void shutdown();
	void update_turning(float zoom_factor);
};