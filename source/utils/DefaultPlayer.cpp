#include "DefaultPlayer.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>

REGISTER_COMPONENT(DefaultPlayer);

using namespace Unigine;

void DefaultPlayer::init()
{
	if (!player.nullCheck())
		player_camera = checked_ptr_cast<Player>(player.get());
	else
		player_camera = Game::getPlayer();

	if (!player_camera)
	{
		Log::error("DefaultSpectator::init(): must be at least one player in world!\n");
		return;
	}

	init_player_controlled = player_camera->isControlled();
	init_mouse_enabled = ControlsApp::isMouseEnabled();

	auto button = Input::MOUSE_BUTTON(mouse_button.get());
	if (button != Input::MOUSE_BUTTON_UNKNOWN)
		spectator_mode_button = button;
}

void DefaultPlayer::update()
{
	if (Console::isActive() || !player_camera || (Game::getPlayer() != player_camera))
		return;

	bool current_state = Input::isMouseButtonPressed(spectator_mode_button);
	if (prev_state != current_state)
	{
		ControlsApp::setMouseEnabled(current_state ? true : init_mouse_enabled);
		player_camera->setControlled(current_state ? true : init_player_controlled);
		prev_state = current_state;
	}

	Input::setMouseCursorHide(current_state);
}

void DefaultPlayer::shutdown()
{
	if (player_camera)
		player_camera->setControlled(init_player_controlled);

	ControlsApp::setMouseEnabled(init_mouse_enabled);
	Input::setMouseCursorHide(false);
}
