#include "Pawn.h"

#include <UnigineConsole.h>
#include <UnigineRender.h>

REGISTER_COMPONENT(Pawn);

#define DAMAGE_EFFECT_TIME 0.5f

using namespace Unigine;
using namespace Math;

void Pawn::init()
{
	player = Game::getPlayer();
	controls = player->getControls();

	default_model_view = player->getCamera()->getModelview();
	damage_effect_timer = 0;
	show_damage_effect();

	Log::message("PAWN: INIT \"%s\"\n", name.get());
}

void Pawn::update()
{
	// get delta time between frames
	float ifps = Game::getIFps();

	// show damage effect
	if (damage_effect_timer > 0)
	{
		damage_effect_timer = Math::clamp(damage_effect_timer - ifps, 0.0f, DAMAGE_EFFECT_TIME);
		show_damage_effect();
	}

	// if console is opened we disable any controls
	if (Console::isActive())
		return;

	// get the direction vector of the mesh from the second column (y axis) of the transformation
	// matrix
	Vec3 direction = node->getWorldTransform().getColumn3(1);

	// checking controls states and changing pawn position and rotation
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

void Pawn::shutdown()
{
	Log::message("PAWN: DEAD!\n");
}

void Pawn::hit(int damage)
{
	// take damage
	health = health - damage;

	// show effect
	damage_effect_timer = DAMAGE_EFFECT_TIME;
	show_damage_effect();

	// destroy
	if (health <= 0)
		node.deleteLater();

	Log::message("PAWN: damage taken (%d) - HP left (%d)\n", damage, health.get());
}

void Pawn::show_damage_effect()
{
	float strength = damage_effect_timer / DAMAGE_EFFECT_TIME;
	Render::setFadeColor(vec4(0.5f, 0, 0, saturate(strength - 0.5f)));
	player->getCamera()->setModelview(default_model_view
		* Mat4(rotateX(Game::getRandomFloat(-5, 5) * strength)
			* rotateY(Game::getRandomFloat(-5, 5) * strength)));
}
