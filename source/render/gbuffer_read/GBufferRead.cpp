#include "GBufferRead.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(GBufferRead)

using namespace Unigine;
using namespace Math;

constexpr int TEXTURE_WIDTH = 1024;
constexpr int TEXTURE_HEIGHT = 1024;


void GBufferRead::init()
{
	viewport = Viewport::create();

	// as we are not interested in the final image, we can stop
	// rendering when G-buffer textures are ready
	viewport->setRenderMode(Viewport::RENDER_DEPTH_GBUFFER);
	viewport->getEventEndOpacityGBuffer().connect(on_viewport_g_buffer_ready_connection, this,
		&GBufferRead::g_buffers_ready_callback);

	Render::getEventEndOpacityGBuffer().connect(on_render_g_buffer_ready_connection, this,
		&GBufferRead::g_buffers_ready_callback);

	// create all textures
	// use Texture::FORMAT_USAGE_RENDER so texture can be used in rendering operations
	albedo_texture = Texture::create();
	albedo_texture->create2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	normal_texture = Texture::create();
	normal_texture->create2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	metalness_texture = Texture::create();
	metalness_texture->create2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	roughness_texture = Texture::create();
	roughness_texture->create2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	depth_texture = Texture::create();
	depth_texture->create2D(TEXTURE_WIDTH, TEXTURE_HEIGHT, Texture::FORMAT_RGBA8,
		Texture::FORMAT_USAGE_RENDER);

	setMode(mode);
}

void GBufferRead::g_buffers_ready_callback()
{
	RenderTargetPtr render_target = Renderer::getPostRenderTarget();

	// save current state and put it to the top of RenderStates saved states stack
	RenderState::saveState();
	// clear current render state, so our render won't be affected by current render settings
	RenderState::clearStates();

	// set textures in which we will render
	render_target->bindColorTexture(0, depth_texture);
	render_target->bindColorTexture(1, albedo_texture);
	render_target->bindColorTexture(2, normal_texture);
	render_target->bindColorTexture(3, metalness_texture);
	render_target->bindColorTexture(4, roughness_texture);

	// enable render target
	render_target->enable();
	{
		// Bind already rendered GBuffer textures to shader
		// we use RenderState::BIND_FRAGMENT to bind textures only for fragment shader
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, Renderer::getTextureOpacityDepth());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, Renderer::getTextureGBufferAlbedo());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 2, Renderer::getTextureGBufferNormal());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 3,
			Renderer::getTextureGBufferShading());

		// this material contains shader, which will render textures, bound to RenderTarget
		// to see code of this shader, go to sample folder and then to materials folder
		// and open fetch_buffers_post.basemat
		fetch_buffers_post_material->renderScreen(Render::PASS_POST);
	}
	render_target->disable();
	render_target->unbindColorTextures();

	// get RenderState from top of inner RenderState stack
	RenderState::restoreState();

	depth_texture->createMipmaps();
	normal_texture->createMipmaps();
	roughness_texture->createMipmaps();
	metalness_texture->createMipmaps();
	albedo_texture->createMipmaps();
}

void GBufferRead::renderNode(const Mat4 &model_view, const Mat4 &projection,
	const NodePtr &to_render) const
{
	// here we will render node with our viewport
	CameraPtr camera = Camera::create();
	camera->setProjection(mat4(projection));
	camera->setModelview(model_view);

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	{
		// here g_buffers_ready_callback will be called, after all g_buffers textures will be ready
		// look at init() function to see how we subscribe on this event
		viewport->renderNode(camera, to_render);
	}
	RenderState::restoreState();
}

void GBufferRead::setMode(GBufferRead::Mode in_mode)
{
	mode = in_mode;
	switch (mode)
	{
	case Mode::RenderNode:
		on_render_g_buffer_ready_connection.setEnabled(false);
		on_viewport_g_buffer_ready_connection.setEnabled(true);
		break;
	case Mode::RenderMainViewport:
		on_render_g_buffer_ready_connection.setEnabled(true);
		on_viewport_g_buffer_ready_connection.setEnabled(false);
		break;
	}
}
