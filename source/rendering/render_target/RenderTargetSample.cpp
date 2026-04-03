// Demonstrates decal-style texture painting. Clicking on an object draws
// sprites at the UV intersection point. Uses render targets to composite
// sprites onto a persistent texture that updates the wall's albedo.

#include "RenderTargetSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

constexpr float DRAW_TIMEOUT = 0.5f;
const vec4 SPRITE_COLOR = {0.f, 0.f, 1.f, 0.7f, ConstexprTag{}};

REGISTER_COMPONENT(RenderTargetSample)

// Sprite textures are loaded; wall's albedo is copied to render target textures.
void RenderTargetSample::init()
{
	// Load all sprite images from file paths into GPU textures
	for (int i = 0; i < sprites.size(); i++)
	{
		String sprite_path = sprites[i].get();
		TexturePtr sprite_texture = Texture::create();
		sprite_texture->load(sprite_path);
		sprite_textures.push_back(std::move(sprite_texture));
	}

	// Get wall object and inherit its material so changes don't affect other instances
	wall = checked_ptr_cast<Object>(wall_param.get());
	wall_material = wall->getMaterialInherit(0);

	// Load the original albedo texture as a CPU image (for dimensions and reset)
	wall_albedo = Image::create();
	wall_albedo->load(wall_material->getTexturePath(wall_material->findTexture("albedo")));

	countdown = DRAW_TIMEOUT;

	const int width = wall_albedo->getWidth();
	const int height = wall_albedo->getHeight();

	render_target = RenderTarget::create();

	// Background texture holds the accumulated sprites (persistent between frames)
	background_texture = Texture::create();
	background_texture->create2D(width, height, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	// Convert CPU image to GPU texture and copy to background
	const TexturePtr albedo_texture = Texture::create();
	albedo_texture->create(wall_albedo);
	copy_texture(background_texture, albedo_texture);

	// Result texture is what the wall actually displays (updated after each sprite draw)
	result_texture = Texture::create();
	result_texture->create2D(width, height, Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_RENDER);

	// Initialize result with background content and assign to wall material
	copy_texture(result_texture, background_texture);
	wall_material->setTexture(wall_material->findTexture("albedo"), result_texture);
}

// Mouse clicks draw sprites at UV intersection; random sprites are added periodically.
void RenderTargetSample::update()
{
	// Handle mouse click painting
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		// Cast a ray from camera through mouse position
		Vec3 p0, p1;
		PlayerPtr player = Game::getPlayer();
		player->getDirectionFromMainWindow(p0, p1, Input::getMousePosition().x,
			Input::getMousePosition().y);

		// Transform ray to wall's local space for intersection test
		Vec3 wall_p0 = wall->getIWorldTransform() * p0;
		Vec3 wall_p1 = wall->getIWorldTransform() * p1;

		// Check if ray hits the wall and get UV coordinates at hit point
		ObjectIntersectionTexCoordPtr intersection = ObjectIntersectionTexCoord::create();
		if (wall->getIntersection(wall_p0, wall_p1, ~0, intersection, nullptr))
		{
			vec4 tex_coord = intersection->getTexCoord();
			draw_sprite(tex_coord.x, tex_coord.y);
		}
	}

	// Auto-spawn sprites at random positions every DRAW_TIMEOUT seconds
	countdown -= Game::getIFps();
	if (countdown <= 0.0f)
	{
		float x = Game::getRandomFloat(0.0, 1.0f);
		float y = Game::getRandomFloat(0.0, 1.0f);
		draw_sprite(x, y);
		countdown = DRAW_TIMEOUT;
	}
}

// Render target and all created textures are released.
void RenderTargetSample::shutdown()
{
	render_target->destroy();
	render_target.clear();

	for (int i = 0; i < sprite_textures.size(); i++)
	{
		if (sprite_textures[i])
		{
			sprite_textures[i]->destroy();
			sprite_textures[i].clear();
		}
	}

	background_texture->destroy();
	background_texture.clear();

	result_texture->destroy();
	result_texture.clear();
}

// Fullscreen blit from source to destination using built-in copy material.
void RenderTargetSample::copy_texture(const TexturePtr &dest, const TexturePtr &src) const
{
	// Preserve current render state to restore after our operation
	RenderState::saveState();
	RenderState::clearStates();

	// Set destination texture as render target output
	render_target->bindColorTexture(0, dest);
	render_target->enable();
	{
		// Use engine's built-in copy shader for simple texture blit
		MaterialPtr material = Materials::findManualMaterial("Unigine::render_copy_2d");
		if (material)
		{
			int color_id = material->findTexture("color");
			material->setTexture(color_id, src);
			// Fullscreen quad renders source texture to destination
			material->renderScreen(Render::PASS_POST);
			material->setTexture(color_id, nullptr);
		}
	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();
}

// Sprite is transformed to UV position and blended onto the result texture.
void RenderTargetSample::draw_sprite(float uv_x, float uv_y)
{
	// Force shader compilation if needed and get shader handle
	ShaderPtr shader = draw_sprite_material->getShaderForce(Render::PASS_POST);
	if (!shader)
	{
		return;
	}

	// Pick a random sprite from the loaded textures
	TexturePtr sprite_texture = sprite_textures[Game::getRandomInt(0, sprite_textures.size() - 1)];

	// Convert UV [0..1] to clip space [-1..1], accounting for Y-flip on some platforms
	const float flip_sign = (Render::isFlipped() ? -1.0f : 1.0f);
	const float translate_x = 2.0f * uv_x - 1.0f;
	const float translate_y = flip_sign * (2.0f * uv_y - 1.0f);

	// Scale sprite to maintain its pixel size relative to background texture
	const float scale_x = itof(sprite_texture->getWidth()) / itof(background_texture->getWidth());
	const float scale_y = itof(sprite_texture->getHeight()) / itof(background_texture->getHeight());

	// Build transform matrix: first scale, then translate to UV position
	const mat4 transform = translate(translate_x, translate_y, 0.0f)
		* scale(scale_x, scale_y, 1.0f);

	RenderState::saveState();
	RenderState::clearStates();

	// Render to result texture (sprite composited over background)
	render_target->bindColorTexture(0, result_texture);
	render_target->enable();
	{
		// Set shader parameters for sprite positioning and tint color
		shader->bind();
		shader->setParameterFloat4x4("ss_transform", transform);
		shader->setParameterFloat4("ss_color", SPRITE_COLOR);
		shader->flushParameters();

		// Slot 0: background to preserve, Slot 1: sprite to blend on top
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, background_texture);
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, sprite_texture);
		draw_sprite_material->renderScreen(Render::PASS_POST);
	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();

	// Copy result back to background so next sprite draws on top of all previous ones
	copy_texture(background_texture, result_texture);

	// Generate mipmaps for proper filtering at distance
	result_texture->createMipmaps();
}
