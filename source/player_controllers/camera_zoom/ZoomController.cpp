// Implements camera zoom by adjusting FOV, render distance scale, and mouse
// sensitivity in sync. Higher zoom narrows FOV while extending LOD distances
// and reducing sensitivity for stable aiming at long range.

#include "ZoomController.h"

#include "UnigineConsole.h"
#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ZoomController);

// Default values are captured from current player and render settings.
void ZoomController::init()
{
	player = checked_ptr_cast<Player>(node);
	if (!player)
	{
		Log::error("ZoomSample::init cannot cast node to player!\n");
	}

	default_FOV = player->getFov();
	default_distance_scale = Render::getDistanceScale();
	default_sensivity = ControlsApp::getMouseSensitivity();

	// Turning speed is only available on Spectator and Actor player types
	if (node->getType() == Node::PLAYER_SPECTATOR)
	{
		PlayerSpectatorPtr player_spectator = checked_ptr_cast<PlayerSpectator>(node);
		default_player_turning = player_spectator->getTurning();
	}
	if (node->getType() == Node::PLAYER_ACTOR)
	{
		PlayerActorPtr player_spectator = checked_ptr_cast<PlayerActor>(node);
		default_player_turning = player_spectator->getTurning();
	}
}

// Global render/input settings are restored to prevent affecting other samples.
void ZoomController::shutdown()
{
	Render::setDistanceScale(default_distance_scale);
	ControlsApp::setMouseSensitivity(default_sensivity);
}

// Camera direction is set toward the target node.
void ZoomController::focus_on_target(NodePtr target)
{
	vec3 dir = vec3(target->getWorldPosition() - node->getWorldPosition());
	dir.normalize();
	player->setViewDirection(dir);
}

// All zoom-dependent parameters are scaled proportionally.
void ZoomController::udpate_zoom_factor(float zoom_factor)
{
	// FOV narrows, LOD extends, sensitivity decreases for stable long-range aiming
	player->setFov(default_FOV / zoom_factor);
	Render::setDistanceScale(default_distance_scale * zoom_factor);
	ControlsApp::setMouseSensitivity(default_sensivity / zoom_factor);

	if (node->getType() == Node::PLAYER_SPECTATOR|| node->getType() == Node::PLAYER_ACTOR)
	{
		update_turning(zoom_factor);
	}
}

// Turning speed is scaled for Spectator/Actor (not available on base Player).
// Type checking is required since Turning property varies by player class.
void ZoomController::update_turning(float zoom_factor)
{
	if (node->getType() == Node::PLAYER_SPECTATOR)
	{
		PlayerSpectatorPtr player_spectator = checked_ptr_cast<PlayerSpectator>(node);
		player_spectator->setTurning(default_player_turning / zoom_factor);
	}
	if (node->getType() == Node::PLAYER_ACTOR)
	{
		PlayerActorPtr player_actor = checked_ptr_cast<PlayerActor>(node);
		player_actor->setTurning(default_player_turning / zoom_factor);
	}
}

void ZoomController::reset()
{
	udpate_zoom_factor(1);
}
