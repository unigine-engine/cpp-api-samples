#include "ZoomController.h"

#include "UnigineConsole.h"
#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ZoomController);

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

void ZoomController::shutdown()
{
	//so settings won't be affected between sessions
	Render::setDistanceScale(default_distance_scale);
	ControlsApp::setMouseSensitivity(default_sensivity);
}

void ZoomController::focus_on_target(NodePtr target)
{
	vec3 dir = vec3(target->getWorldPosition() - node->getWorldPosition());
	dir.normalize();
	player->setViewDirection(dir);
}

void ZoomController::udpate_zoom_factor(float zoom_factor)
{
	player->setFov(default_FOV / zoom_factor);
	Render::setDistanceScale(default_distance_scale * zoom_factor);
	ControlsApp::setMouseSensitivity(default_sensivity / zoom_factor);

	if (node->getType() == Node::PLAYER_SPECTATOR|| node->getType() == Node::PLAYER_ACTOR)
	{
		update_turning(zoom_factor);
	}
}

void ZoomController::update_turning(float zoom_factor)
{
	//Turning determines speed at which the Player is rotated. It should be lowered and heightened depending on zoom factor
	// ZoomController has been made for the base Player class so it could work with any Player derived class But not every Player has a Turning property
	// Because of that we should regulate Turning depeping on player node type.
	//There is no work around this situation and since changing behavior depending on class type is bad practice this functionality has been hidden from public interface

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
