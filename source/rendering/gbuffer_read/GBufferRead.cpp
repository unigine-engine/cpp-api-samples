// Extracts G-buffer components (albedo, normal, metalness, roughness, depth)
// into separate textures after the deferred pass. Hooks into the end of
// G-buffer rendering to copy data before lighting calculations.

#include "GBufferRead.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(GBufferRead)

using namespace Unigine;
using namespace Math;

constexpr int TEXTURE_WIDTH = 1024;
constexpr int TEXTURE_HEIGHT = 1024;

// Viewport and output textures are created; event callbacks are registered for both modes.
void GBufferRead::init()
{
	viewport = Viewport::create();

	// As we are not interested in the final image, we can stop
	// rendering when G-buffer textures are ready
	viewport->setRenderMode(Viewport::RENDER_DEPTH_GBUFFER);
	viewport->getEventEndOpacityGBuffer().connect(on_viewport_g_buffer_ready_connection, this,
		&GBufferRead::g_buffers_ready_callback);

	Render::getEventEndOpacityGBuffer().connect(on_render_g_buffer_ready_connection, this,
		&GBufferRead::g_buffers_ready_callback);

	// Create all textures
	// Use Texture::FORMAT_USAGE_RENDER so texture can be used in rendering operations
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

// Extracts G-buffer components into separate textures using a fullscreen shader pass.
void GBufferRead::g_buffers_ready_callback()
{
	RenderTargetPtr render_target = Renderer::getPostRenderTarget();

	// Save current state and put it to the top of RenderStates saved states stack
	RenderState::saveState();
	// Clear current render state, so our render won't be affected by current render settings
	RenderState::clearStates();

	// Set textures in which we will render
	render_target->bindColorTexture(0, depth_texture);
	render_target->bindColorTexture(1, albedo_texture);
	render_target->bindColorTexture(2, normal_texture);
	render_target->bindColorTexture(3, metalness_texture);
	render_target->bindColorTexture(4, roughness_texture);

	// Enable render target
	render_target->enable();
	{
		// Bind already rendered GBuffer textures to shader
		// We use RenderState::BIND_FRAGMENT to bind textures only for fragment shader
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, Renderer::getTextureOpacityDepth());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, Renderer::getTextureGBufferAlbedo());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 2, Renderer::getTextureGBufferNormal());
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 3,
			Renderer::getTextureGBufferShading());

		// This material contains shader which will render textures bound to RenderTarget.
		// To see code of this shader, go to sample folder and then to materials folder
		// and open fetch_buffers_post.basemat
		fetch_buffers_post_material->renderScreen(Render::PASS_POST);
	}
	render_target->disable();
	render_target->unbindColorTextures();

	// Get RenderState from top of inner RenderState stack
	RenderState::restoreState();

	depth_texture->createMipmaps();
	normal_texture->createMipmaps();
	roughness_texture->createMipmaps();
	metalness_texture->createMipmaps();
	albedo_texture->createMipmaps();
}

// Renders a single node using custom camera matrices and triggers G-buffer extraction.
void GBufferRead::renderNode(const Mat4 &model_view, const Mat4 &projection,
	const NodePtr &to_render) const
{
	// Temporary camera is created with provided matrices for the node render
	CameraPtr camera = Camera::create();
	camera->setProjection(mat4(projection));
	camera->setModelview(model_view);

	RenderState::saveState();
	RenderState::clearStates();
	RenderState::setViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	{
		// Here g_buffers_ready_callback will be called after all G-buffer textures are ready.
		// See init() function for event subscription.
		viewport->renderNode(camera, to_render);
	}
	RenderState::restoreState();
}

// Switches between RenderNode (custom viewport) and RenderMainViewport (scene camera) modes.
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
