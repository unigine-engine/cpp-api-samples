#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class BoatControl : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BoatControl, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, max_speed, 10.0f);
	PROP_PARAM(Float, max_rotation, 10.0f);
	PROP_PARAM(Float, acceleration_factor, 0.5f);
	PROP_PARAM(Float, min_speed_for_rot, -1.0f)
	PROP_PARAM(Toggle, invert_rear_dir, false);

private:
	void init();
	void update();
	void shutdown();

	Unigine::Math::Scalar forward = 0.0f;
	float rotation = 0.0f;
};