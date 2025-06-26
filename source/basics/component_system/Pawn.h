#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineControls.h>
#include <UnigineGame.h>

class Pawn : public Unigine::ComponentBase
{
public:
	// methods
	COMPONENT(Pawn, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// property name
	PROP_NAME("pawn");

	// parameters
	PROP_PARAM(String, name, "Pawn1");    // Pawn's name
	PROP_PARAM(Int, health, 200);         // health points
	PROP_PARAM(Float, move_speed, 4.0f);  // move speed (meters/s)
	PROP_PARAM(Float, turn_speed, 90.0f); // turn speed (deg/s)

	// methods
	void hit(int damage); // decrease some health points from the pawn

protected:
	// world main loop
	void init();
	void update();
	void shutdown();

private:
	Unigine::ControlsPtr controls;
	Unigine::PlayerPtr player;

	float damage_effect_timer;
	Unigine::Math::Mat4 default_model_view;

	void show_damage_effect();
};
