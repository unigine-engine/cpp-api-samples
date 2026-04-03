#pragma once
#include <UnigineComponentSystem.h>

// Toggleable controller comparing movement with and without IFps scaling.
// When use_ifps is false, movement speed depends on frame rate.
// When use_ifps is true, movement is frame-rate independent (consistent speed).
class IFpsMovementController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(IFpsMovementController, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component controls node movement, optionally using Game::getIfps() for it"
		"depending on the parameter value");

	COMPONENT_UPDATE(update);

private:
	// Parameters
	PROP_PARAM(Toggle, use_ifps, false, "Use IFps");       // Enable frame-rate independent movement
	PROP_PARAM(Float, movement_speed, 1.0f, "Movement Speed"); // Base movement speed

	Unigine::Math::Vec3 current_dir = Unigine::Math::Vec3_right; // Current movement direction

	void update();
};
