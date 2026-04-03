// Prevents weapon from clipping through walls by rendering it with a separate
// near-plane camera. The weapon is rendered to a texture after the main scene,
// then composited on top during post-materials. Uses dual-camera setup where
// the weapon camera tracks the main camera's transform.

#include "WeaponClipping.h"

#include <UnigineBase.h>
#include <UnigineMathLibVec4.h>
#include <UnigineRender.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(WeaponClipping);

using namespace Unigine;
using namespace Math;

// Mouse is grabbed; viewport and texture are created for weapon layer rendering.
void WeaponClipping::init()
{
	// Store current mouse mode for restoration on shutdown
	init_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	// Cache window dimensions for texture sizing
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	current_width = window_size.x;
	current_height = window_size.y;

	// Get player references from component parameters
	player = checked_ptr_cast<Player>(camera.get());
	weapon_player = checked_ptr_cast<Player>(weapon_camera.get());

	// Create dedicated viewport for weapon-only rendering
	viewport = Viewport::create();
	// Include both node and world lights for proper weapon illumination
	viewport->setNodeLightUsage(Viewport::USAGE_NODE_LIGHT | Viewport::USAGE_WORLD_LIGHT);
	// Skip velocity buffer since weapon layer doesn't need motion vectors
	viewport->setSkipFlags(Viewport::SKIP_VELOCITY_BUFFER);

	// Create texture to hold rendered weapon layer
	texture = Texture::create();
	create_texture_2D(*texture);

	// Subscribe to window resize for texture recreation
	WindowManager::getMainWindow()->getEventResized().connect(this,
		&WeaponClipping::update_screen_size);

	// Subscribe to post-materials event for compositing weapon onto scene
	Render::getEventBeginPostMaterials().connect(this, &WeaponClipping::render_callback);
}

// Weapon camera is synchronized with main camera's position and orientation.
void WeaponClipping::update()
{
	// Inverse modelview gives world transform; weapon camera follows main camera exactly
	weapon_player->setTransform(player->getCamera()->getIModelview());
}

// Render target texture is created matching current screen dimensions.
void WeaponClipping::create_texture_2D(Unigine::Texture &in_texture)
{
	// RGBA8 with alpha channel for transparent background around weapon
	in_texture.create2D(current_width, current_height, Texture::FORMAT_RGBA8,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
}

// Texture is recreated when window dimensions change.
void WeaponClipping::update_screen_size()
{
	ivec2 window_size = WindowManager::getMainWindow()->getSize();
	int app_width = window_size.x;
	int app_height = window_size.y;
	if (app_width != current_width || app_height != current_height)
	{
		current_width = app_width;
		current_height = app_height;
		// Recreate texture to match new screen size
		create_texture_2D(*texture);
	}
}

// Mouse handle mode is restored to its original state.
void WeaponClipping::shutdown()
{
	Input::setMouseHandle(init_handle);
}

// Weapon is rendered to texture after main scene update is complete.
void WeaponClipping::post_update()
{
	// Only render weapon for the expected player camera
	if (Game::getPlayer() != player)
		return;

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, current_width, current_height);

	// Get temporary render target for weapon layer
	auto target = Render::getTemporaryRenderTarget();
	target->bindColorTexture(0, texture);

	target->enable();
	{
		// Disable lens flares in weapon layer to avoid visual artifacts
		bool flare = Render::isLightsLensFlares();
		Render::setLightsLensFlares(false);

		// Clear to transparent black so only weapon pixels are visible
		RenderState::clearBuffer(RenderState::BUFFER_ALL, Math::vec4_zero);

		// Render weapon using dedicated camera with custom near clip plane
		if (texture)
		{
			// Flag prevents recursive rendering in render_callback
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

// Weapon texture is composited onto main render during post-materials phase.
void WeaponClipping::render_callback()
{
	// Only composite for the expected player camera
	if (Game::getPlayer() != player)
		return;

	// Skip compositing while rendering weapon to avoid infinite recursion
	if (is_rendering_weapon)
	{
		return;
	}

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, current_width, current_height);

	// Bind main color buffer as output target
	auto target = Render::getTemporaryRenderTarget();
	target->bindColorTexture(0, Renderer::getTextureColor());

	target->enable();
	{
		// Alpha blending overlays weapon on scene; transparent areas show scene through
		RenderState::setBlendFunc(RenderState::BLEND_SRC_ALPHA,
			RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

		// Blit weapon texture onto main render using copy material
		if (texture)
			Render::renderScreenMaterial("Unigine::render_copy_2d", texture);
	}
	target->disable();

	target->unbindColorTexture(0);
	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(target);
}
