#include "RenderTargetSample.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

constexpr float DRAW_TIMEOUT = 0.5f;
const vec4 SPRITE_COLOR = {0.f, 0.f, 1.f, 0.7f, ConstexprTag{}};

REGISTER_COMPONENT(RenderTargetSample)

void RenderTargetSample::init()
{
	for (int i = 0; i < sprites.size(); i++)
	{
		String sprite_path = sprites[i].get();
		TexturePtr sprite_texture = Texture::create();
		sprite_texture->load(sprite_path);
		sprite_textures.push_back(std::move(sprite_texture));
	}

	wall = checked_ptr_cast<Object>(wall_param.get());
	wall_material = wall->getMaterialInherit(0);
	wall_albedo = Image::create();
	wall_albedo->load(wall_material->getTexturePath(wall_material->findTexture("albedo")));

	countdown = DRAW_TIMEOUT;

	const int width = wall_albedo->getWidth();
	const int height = wall_albedo->getHeight();

	render_target = RenderTarget::create();

	background_texture = Texture::create();
	background_texture->create2D(width, height, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	const TexturePtr albedo_texture = Texture::create();
	albedo_texture->create(wall_albedo);
	copy_texture(background_texture, albedo_texture);

	result_texture = Texture::create();
	result_texture->create2D(width, height, Texture::FORMAT_RGBA8, Texture::FORMAT_USAGE_RENDER);

	copy_texture(result_texture, background_texture);
	wall_material->setTexture(wall_material->findTexture("albedo"), result_texture);
}

void RenderTargetSample::update()
{
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		Vec3 p0, p1;
		PlayerPtr player = Game::getPlayer();
		player->getDirectionFromMainWindow(p0, p1, Input::getMousePosition().x,
			Input::getMousePosition().y);
		Vec3 wall_p0 = wall->getIWorldTransform() * p0;
		Vec3 wall_p1 = wall->getIWorldTransform() * p1;

		ObjectIntersectionTexCoordPtr intersection = ObjectIntersectionTexCoord::create();
		if (wall->getIntersection(wall_p0, wall_p1, ~0, intersection, nullptr))
		{
			vec4 tex_coord = intersection->getTexCoord();
			draw_sprite(tex_coord.x, tex_coord.y);
		}
	}

	countdown -= Game::getIFps();
	if (countdown <= 0.0f)
	{
		float x = Game::getRandomFloat(0.0, 1.0f);
		float y = Game::getRandomFloat(0.0, 1.0f);
		draw_sprite(x, y);
		countdown = DRAW_TIMEOUT;
	}
}

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

void RenderTargetSample::copy_texture(const TexturePtr &dest, const TexturePtr &src) const
{
	RenderState::saveState();
	RenderState::clearStates();
	render_target->bindColorTexture(0, dest);
	render_target->enable();
	{
		MaterialPtr material = Materials::findManualMaterial("Unigine::render_copy_2d");
		if (material)
		{
			int color_id = material->findTexture("color");
			material->setTexture(color_id, src);
			material->renderScreen(Render::PASS_POST);
			material->setTexture(color_id, nullptr);
		}
	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();
}

void RenderTargetSample::draw_sprite(float uv_x, float uv_y)
{
	ShaderPtr shader = draw_sprite_material->getShaderForce(Render::PASS_POST);
	if (!shader)
	{
		return;
	}

	TexturePtr sprite_texture = sprite_textures[Game::getRandomInt(0, sprite_textures.size() - 1)];

	const float flip_sign = (Render::isFlipped() ? -1.0f : 1.0f);
	const float translate_x = 2.0f * uv_x - 1.0f;
	const float translate_y = flip_sign * (2.0f * uv_y - 1.0f);
	const float scale_x = itof(sprite_texture->getWidth()) / itof(background_texture->getWidth());
	const float scale_y = itof(sprite_texture->getHeight()) / itof(background_texture->getHeight());

	const mat4 transform = translate(translate_x, translate_y, 0.0f)
		* scale(scale_x, scale_y, 1.0f);

	RenderState::saveState();
	RenderState::clearStates();
	render_target->bindColorTexture(0, result_texture);
	render_target->enable();
	{
		shader->bind();
		shader->setParameterFloat4x4("ss_transform", transform);
		shader->setParameterFloat4("ss_color", SPRITE_COLOR);
		shader->flushParameters();

		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, background_texture);
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, sprite_texture);
		draw_sprite_material->renderScreen(Render::PASS_POST);
	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();

	copy_texture(background_texture, result_texture);

	result_texture->createMipmaps();
}
