#include "GBufferWrite.h"

#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(GBufferWrite)


// this callback will called after g-buffer textures is ready
void GBufferWrite::g_buffers_ready_callback()
{
	// get shader from material to set parameters and bind it in exact time we want
	// we use getShaderForce to get it instantly, and if shader is not compiled, engine will compile
	// it and we can use it instantly
	ShaderPtr shader = modify_buffer_material->getShaderForce(Render::PASS_POST);
	if (!shader)
	{
		return;
	}

	RenderTargetPtr render_target = Renderer::getPostRenderTarget();

	// because this callback is called after g-buffer textures are ready,
	// we can freely get them
	TexturePtr albedo = Renderer::getTextureGBufferAlbedo();
	TexturePtr normal = Renderer::getTextureGBufferNormal();
	TexturePtr shading = Renderer::getTextureGBufferShading();

	// engine will allocate new texture, or just use one from its internal pool,
	// so its better to use this method than create new textures
	// but we need to release this textures explicitly ourselves
	TexturePtr albedo_temp = Render::getTemporaryTexture(albedo);
	TexturePtr normal_temp = Render::getTemporaryTexture(normal);
	TexturePtr shading_temp = Render::getTemporaryTexture(shading);


	RenderState::saveState();
	RenderState::clearStates();

	// set textures with flag BIND_FRAGMENT, so this textures can be usable only from fragment
	// shader these textures correspond to this shader code in our material STRUCT_FRAG_BEGIN
	// 	INIT_MRT(float4, OUT_ALBEDO)
	// 	INIT_MRT(float4, OUT_NORMAL)
	// 	INIT_MRT(float4, OUT_SHADING)
	// STRUCT_FRAG_END
	// MRT stands for Multiple Render Target, and we use it to render multiple textures in single
	// draw call
	render_target->bindColorTexture(0, albedo_temp);
	render_target->bindColorTexture(1, normal_temp);
	render_target->bindColorTexture(2, shading_temp);
	render_target->enable();
	{
		// bind shader and set its parameters (look into modify_buffers_post.basemat
		// to find out how you can use this parameters in shader
		shader->bind();
		// these parameters correspond with these lines in shader
		// CBUFFER(parameters)
		//	UNIFORM float ss_influence;
		//	UNIFORM float ss_plastic;
		//	UNIFORM float4 ss_color;
		// END
		shader->setParameterFloat("ss_influence", influence);
		shader->setParameterFloat("ss_plastic", plastic);
		shader->setParameterFloat4("ss_color", color);
		shader->flushParameters();

		// here we bind textures to shader so we can read from it
		// this lines correspond to these shader lines
		// INIT_TEXTURE(0, TEX_ALBEDO)
		// INIT_TEXTURE(1, TEX_NORMAL)
		// INIT_TEXTURE(2, TEX_SHADING)
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, albedo);
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 1, normal);
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 2, shading);

		// now we can start our render
		modify_buffer_material->renderScreen(Render::PASS_POST);
	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();

	// here we set new g-buffer textures
	albedo_temp->swap(albedo);
	normal_temp->swap(normal);
	shading_temp->swap(shading);

	// release temporary textures
	Render::releaseTemporaryTexture(albedo_temp);
	Render::releaseTemporaryTexture(normal_temp);
	Render::releaseTemporaryTexture(shading_temp);
}

void GBufferWrite::init()
{
	Render::getEventEndOpacityGBuffer().connect(this, &GBufferWrite::g_buffers_ready_callback);

	sample_description_window.createWindow();

	color_label = sample_description_window.addLabel("Color: ");
	plastic_label = sample_description_window.addLabel("Plastic: ");
	influence_label = sample_description_window.addLabel("Influence: ");
}

void GBufferWrite::update()
{
	const VectorStack<vec4> colors = {vec4(1.0f, 1.0f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 1.0f, 1.0f, 1.0f), vec4(0.0f, 0.0f, 1.0f, 1.0f), vec4(1.0f, 0.0f, 1.0f, 1.0f)};

	const float time = Game::getTime();
	const float ifps = Game::getIFps();

	influence = saturate(Math::sin(time / 2.0f) + 1.0f);
	plastic = (Math::cos(time / 5.0f) + 1.0f) / 2.0f;

	int color_index = static_cast<int>(time / 4.0f) % colors.size();

	color = lerp(color, colors[color_index], 0.2f * ifps);

	color_label->setText(
		String::format("Color: %0.2f %0.2f %0.2f %0.2f", color.x, color.y, color.z, color.w));
	influence_label->setText(String::format("Influence: %0.2f", influence));
	plastic_label->setText(String::format("Plastic: %0.2f", plastic));
}

void GBufferWrite::shutdown()
{
	sample_description_window.shutdown();
}
