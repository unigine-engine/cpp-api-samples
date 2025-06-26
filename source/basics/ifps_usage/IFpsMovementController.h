#pragma once
#include <UnigineComponentSystem.h>

class IFpsMovementController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(IFpsMovementController, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component controls node movement, optionally using Game::getIfps() for it"
		"depending on the parameter value");

	COMPONENT_UPDATE(update);

private:
	PROP_PARAM(Toggle, use_ifps, false, "Use IFps");
	PROP_PARAM(Float, movement_speed, 1.0f, "Movement Speed");
	Unigine::Math::Vec3 current_dir = Unigine::Math::Vec3_right;

	void update();
};
