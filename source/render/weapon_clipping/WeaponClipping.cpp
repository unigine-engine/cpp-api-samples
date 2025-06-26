#include "WeaponClipping.h"

#include <UnigineBase.h>
#include <UnigineMathLibVec4.h>
#include <UnigineRender.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(WeaponClipping);

using namespace Unigine;
using namespace Math;

void WeaponClipping::init()
{
	init_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	current_width = window_size.x;
	current_height = window_size.y;

	player = checked_ptr_cast<Player>(camera.get());
	weapon_player = checked_ptr_cast<Player>(weapon_camera.get());

	viewport = Viewport::create();
	viewport->setNodeLightUsage(Viewport::USAGE_NODE_LIGHT | Viewport::USAGE_WORLD_LIGHT);
	viewport->setSkipFlags(Viewport::SKIP_VELOCITY_BUFFER);

	texture = Texture::create();
	create_texture_2D(*texture);

	WindowManager::getMainWindow()->getEventResized().connect(this,
		&WeaponClipping::update_screen_size);

	Render::getEventBeginPostMaterials().connect(this, &WeaponClipping::render_callback);
}

void WeaponClipping::update()
{
	weapon_player->setTransform(player->getCamera()->getIModelview());
}

void WeaponClipping::create_texture_2D(Unigine::Texture &in_texture)
{
	in_texture.create2D(current_width, current_height, Texture::FORMAT_RGBA8,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
}

void WeaponClipping::update_screen_size()
{
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	int app_width = window_size.x;
	int app_height = window_size.y;
	if (app_width != current_width || app_height != current_height)
	{
		current_width = app_width;
		current_height = app_height;
		create_texture_2D(*texture);
	}
}

void WeaponClipping::shutdown()
{
	Input::setMouseHandle(init_handle);
}

void WeaponClipping::post_update()
{
	if (Game::getPlayer() != player)
		return;

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, current_width, current_height);
	auto target = Render::getTemporaryRenderTarget();
	target->bindColorTexture(0, texture);

	target->enable();
	{
		bool flare = Render::isLightsLensFlares();
		Render::setLightsLensFlares(false);
		RenderState::clearBuffer(RenderState::BUFFER_ALL, Math::vec4_zero);

		// render near plane with weapon to texture
		if (texture)
		{
			is_rendering_weapon = true;
			viewport->renderTexture2D(weapon_player->getCamera(), texture);
			is_rendering_weapon = false;
		}

		Render::setLightsLensFlares(flare);
	}

	target->disable();

	target->unbindColorTexture(0);
	RenderState::restoreState();
}

void WeaponClipping::render_callback()
{
	if (Game::getPlayer() != player)
		return;

	if (is_rendering_weapon)
	{
		// skip render to screen when we rendering weapon into custom texture
		return;
	}

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, current_width, current_height);

	auto target = Render::getTemporaryRenderTarget();
	target->bindColorTexture(0, Renderer::getTextureColor());

	target->enable();
	{
		RenderState::setBlendFunc(RenderState::BLEND_SRC_ALPHA,
			RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

		// render texture with weapon to screen
		if (texture)
			Render::renderScreenMaterial("Unigine::render_copy_2d", texture);
	}
	target->disable();

	target->unbindColorTexture(0);
	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(target);
}
