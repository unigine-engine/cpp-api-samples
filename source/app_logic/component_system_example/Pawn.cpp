// Player-controlled pawn with health and movement. Uses Controls states for input
// (works with keyboard, gamepad, or custom bindings). Damage triggers visual feedback:
// red screen overlay via Render::setFadeColor and camera shake via Modelview matrix.

#include "Pawn.h"

#include <UnigineConsole.h>
#include <UnigineRender.h>

REGISTER_COMPONENT(Pawn);

#define DAMAGE_EFFECT_TIME 0.5f

using namespace Unigine;
using namespace Math;

// Player reference and controls are cached, damage effect is initialized.
void Pawn::init()
{
	player = Game::getPlayer();
	controls = player->getControls();

	default_model_view = player->getCamera()->getModelview();
	damage_effect_timer = 0;
	show_damage_effect();

	Log::message("PAWN: INIT \"%s\"\n", name.get());
}

// Movement is processed based on control states, damage effect is updated.
void Pawn::update()
{
	// Get delta time between frames
	float ifps = Game::getIFps();

	// Show damage effect
	if (damage_effect_timer > 0)
	{
		damage_effect_timer = Math::clamp(damage_effect_timer - ifps, 0.0f, DAMAGE_EFFECT_TIME);
		show_damage_effect();
	}

	// If console is opened all controls are disabled
	if (Console::isActive())
		return;

	// Get the direction vector of the mesh from the second column (Y axis) of the transformation
	// matrix
	Vec3 direction = node->getWorldTransform().getColumn3(1);

	// Checking controls states and changing pawn position and rotation
	if (controls->getState(Controls::STATE_FORWARD) || controls->getState(Controls::STATE_TURN_UP))
		node->setWorldPosition(node->getWorldPosition() + direction * move_speed * ifps);

	if (controls->getState(Controls::STATE_BACKWARD)
		|| controls->getState(Controls::STATE_TURN_DOWN))
		node->setWorldPosition(node->getWorldPosition() - direction * move_speed * ifps);

	if (controls->getState(Controls::STATE_MOVE_LEFT)
		|| controls->getState(Controls::STATE_TURN_LEFT))
		node->setWorldRotation(node->getWorldRotation() * quat(0.0f, 0.0f, turn_speed * ifps));

	if (controls->getState(Controls::STATE_MOVE_RIGHT)
		|| controls->getState(Controls::STATE_TURN_RIGHT))
		node->setWorldRotation(node->getWorldRotation() * quat(0.0f, 0.0f, -turn_speed * ifps));
}

// Pawn destruction is logged.
void Pawn::shutdown()
{
	Log::message("PAWN: DEAD!\n");
}

// Damage is applied to health, visual effect is triggered, node is destroyed if health depleted.
void Pawn::hit(int damage)
{
	// Take damage
	health = health - damage;

	// Show effect
	damage_effect_timer = DAMAGE_EFFECT_TIME;
	show_damage_effect();

	// Destroy
	if (health <= 0)
		node.deleteLater();

	Log::message("PAWN: damage taken (%d) - HP left (%d)\n", damage, health.get());
}

// Red overlay and camera shake are applied based on remaining effect timer.
void Pawn::show_damage_effect()
{
	// Visual damage feedback: red screen overlay + camera shake.
	// Effect strength decreases over time (damage_effect_timer counts down).
	float strength = damage_effect_timer / DAMAGE_EFFECT_TIME;
	Render::setFadeColor(vec4(0.5f, 0, 0, saturate(strength - 0.5f)));
	// Apply random rotation to camera for shake effect
	player->getCamera()->setModelview(default_model_view
		* Mat4(rotateX(Game::getRandomFloat(-5, 5) * strength)
			* rotateY(Game::getRandomFloat(-5, 5) * strength)));
}
