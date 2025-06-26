#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineInput.h>

class CarControl: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CarControl, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, max_speed, 10.f);
	PROP_PARAM(Float, max_rotation, 50.f);
	PROP_PARAM(Float, acceleration_factor, 0.5f);
	PROP_PARAM(Toggle, use_arrows, false);

private:
	void init();
	void update();
	void shutdown();

	int get_movement_command() const;
	int get_rotate_command() const;
	void init_input_keys();

	Unigine::Math::Scalar forward = 0.f;
	float rotation = 0.f;
	Unigine::Input::KEY key_forward;
	Unigine::Input::KEY key_back;
	Unigine::Input::KEY key_left;
	Unigine::Input::KEY key_right;
};
