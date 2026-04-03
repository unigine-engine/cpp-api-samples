// Implements two-point perspective by shifting the projection matrix.
// Vertical lines remain parallel while looking up/down. Uses a dummy
// player for modified projection during rendering while input/movement
// uses the original player.

#include "CameraShiftController.h"

#include <UnigineEngine.h>
#include <UnigineGame.h>
#include <UnigineObjects.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(CameraShiftController);

using namespace Unigine;
using namespace Math;

void CameraShiftController::init()
{
	// Get controlled PlayerSpectator from node
	player = checked_ptr_cast<PlayerSpectator>(node);
	if (!player)
		Log::error("CameraShiftController::init(): component node is not a PlayerSpectator!\n");

	float fov = player->getFov();

	// Limit vertical angles so frustum does not flip when shifting
	player->setMaxThetaAngle(89.0f - fov / 2.0f);
	player->setMinThetaAngle(-89.0f + fov / 2.0f);

	// Use the original player for input and movement,
	// create a dummy player for overriding projection matrix
	dummy_player = PlayerDummy::create();
	dummy_player->setCamera(player->getCamera()->clone());

	// Hook into rendering callbacks
	Engine::get()->getEventBeginRender().connect(this, &CameraShiftController::pre_render_callback);
	Engine::get()->getEventBeginSwap().connect(this, &CameraShiftController::post_render_callback);
}

void CameraShiftController::update()
{
	if (!shift_enabled || Game::getPlayer() != player)
		return;

	// Adjust projection matrix for two-point perspective
	auto projection = player->getProjection();
	float znear = player->getZNear();

	auto dir = player->getViewDirection();

	float beta = Math::asin(dir.z);
	float bottom = znear * Math::tan(-player->getFov() * 0.5f * Consts::DEG2RAD + beta);
	float top = znear * Math::tan(player->getFov() * 0.5f * Consts::DEG2RAD + beta);

	projection.m12 = (top + bottom) / (top - bottom);
	projection.m11 = 2 * znear / (top - bottom);

	// Apply new projection and flatten vertical tilt
	dummy_player->setProjection(projection);
	dir.z = 0;
	dummy_player->setViewDirection(dir);
	dummy_player->setWorldPosition(player->getWorldPosition());
}

void CameraShiftController::shutdown()
{
	dummy_player.deleteLater();
}

void CameraShiftController::pre_render_callback()
{
	// Switch to dummy player before rendering
	if (shift_enabled && Game::getPlayer() == player)
	{
		Game::setPlayer(dummy_player);
		need_reset_player = true;
	}
}

void CameraShiftController::post_render_callback()
{
	// Restore original player after rendering
	if (need_reset_player)
	{
		Game::setPlayer(player);
		need_reset_player = false;
	}
}
