#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineControls.h>
#include <UnigineGame.h>

// Player-controlled entity with health, movement, and damage feedback.
// Demonstrates PROP_PARAM for exposing variables to the editor,
// and visual feedback effects (screen shake, color flash on damage).
class Pawn : public Unigine::ComponentBase
{
public:
	// Component macros
	COMPONENT(Pawn, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Property name
	PROP_NAME("pawn");

	// Parameters
	PROP_PARAM(String, name, "Pawn1");    // Pawn's display name
	PROP_PARAM(Int, health, 200);         // Health points
	PROP_PARAM(Float, move_speed, 4.0f);  // Movement speed (meters/s)
	PROP_PARAM(Float, turn_speed, 90.0f); // Turn speed (deg/s)

	// Methods
	void hit(int damage); // Applies damage and triggers visual feedback

protected:
	// World main loop
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
