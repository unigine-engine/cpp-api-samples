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
	COMPONENT_UPDATE_PHYSICS(update_physics);
	COMPONENT_SHUTDOWN(shutdown);

private:
	PROP_PARAM(Toggle, use_update, false, "Use update function");
	PROP_PARAM(Float, linear_force, 5.0f, "Linear force applied to body");
	Unigine::BodyRigidPtr rigid_body;
	float current_force = 0.0f;

	void init();
	void update_physics();
	void update();
	void movement();
	void shutdown();

private:
	bool visualizer_enabled;
};
