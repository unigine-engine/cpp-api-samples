// Compares physics force application in update() vs update_physics().
// update() runs at render framerate (variable), update_physics() runs at
// fixed physics timestep for consistent behavior regardless of frame rate.

#pragma once
#include <UnigineComponentSystem.h>

class UpdatePhysicsUsageController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UpdatePhysicsUsageController, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component controlls node physics movement and uses update and update_physics for it"
	"depending on parameter value");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	// Fixed timestep update synchronized with physics simulation
	COMPONENT_UPDATE_PHYSICS(update_physics);
	COMPONENT_SHUTDOWN(shutdown);

private:
	// When true, forces are applied in update() (variable rate)
	// When false, forces are applied in update_physics() (fixed rate)
	PROP_PARAM(Toggle, use_update, false, "Use update function");
	// Magnitude of horizontal force applied to body
	PROP_PARAM(Float, linear_force, 5.0f, "Linear force applied to body");
	Unigine::BodyRigidPtr rigid_body;
	// Current force direction (flips at boundaries)
	float current_force = 0.0f;

	void init();
	void update_physics();
	void update();
	// Shared movement logic called from either update method
	void movement();
	void shutdown();

private:
	// Original visualizer state for restoration
	bool visualizer_enabled;
};
