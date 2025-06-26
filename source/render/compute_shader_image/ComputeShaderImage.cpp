#include "ComputeShaderImage.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ComputeShaderImage)

void ComputeShaderImage::init()
{
	dynamic_texture = Texture::create();
	dynamic_texture->create2D(1024, 1024, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::FORMAT_USAGE_RENDER);

	// Apply dynamic texture into albedo slot
	int albedo_id = target_material->findTexture("albedo");
	if (albedo_id != -1)
		target_material->setTexture(albedo_id, dynamic_texture);

	render_target = RenderTarget::create();
}

void ComputeShaderImage::update()
{
	if (Render::isFirstFrame())
	{
		render_target->bindColorTexture(0, dynamic_texture);
		render_target->enable();
		RenderState::setBufferMask(0, RenderState::BUFFER_ALL);
		RenderState::clearBuffer(RenderState::BUFFER_ALL, vec4_zero, 0.f);
		render_target->disable();
		render_target->unbindColorTextures();
		return;
	}

	float ifps = Game::getIFps();

	int id = compute_material->findParameter("ifps");
	compute_material->setParameterFloat(id, ifps);

	render_target->bindUnorderedAccessTexture(0, dynamic_texture, false);
	render_target->enableCompute();

	int group_threads_x = 1024 / 32;
	int group_threads_y = 1024 / 32;

	compute_material->renderCompute(Render::PASS_POST, group_threads_x, group_threads_y);

	render_target->disable();
	render_target->unbindUnorderedAccessTextures();
}

void ComputeShaderImage::shutdown()
{
	dynamic_texture->destroy();
	dynamic_texture.clear();

	render_target->destroy();
	render_target.clear();
}
