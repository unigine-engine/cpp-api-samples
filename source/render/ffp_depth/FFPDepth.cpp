#include "FFPDepth.h"

#include <UnigineFfp.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(FFPDepth)

void FFPDepth::init()
{
	Render::getEventEndVisualizer().connect(this, &FFPDepth::render_lines_callback);
}

void FFPDepth::render_lines_callback()
{
	RenderTargetPtr render_target = Renderer::getRenderTarget();
	TexturePtr texture = Renderer::getTextureColor();

	RenderState::saveState();
	RenderState::clearStates();

	// bind color texture
	render_target->bindColorTexture(0, texture);
	// bind depth texture
	// in this callback depth texture is already bound to scene

	render_target->enable();
	{
		RenderState::setDepthFunc(RenderState::DEPTH_GEQUAL);
		RenderState::setBlendFunc(RenderState::BLEND_SRC_ALPHA, RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
		RenderState::flushStates();

		Ffp::enable();

		// pixel_color = vertex_color * this_texture_color
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, Render::getWhite2DTexture());

		mat4 projection;
		// reverse depth and convert from [-1..1] to [0..1] range
		const mat4 offset = translate(0.f, 0.f, 0.5f) * scale(1.f, 1.f, 0.5f);
		mul(projection, offset, reverseDepthProjection((Renderer::getProjectionWithoutTAA())));

		Ffp::setTransform(projection);

		auto model_view = Renderer::getModelview();

		const unsigned col_argb = 0xffff0000;

		Ffp::beginLines();
		Ffp::addLines(1);

		Ffp::Vertex vert;

		// transform to camera space
		vec3(model_view * vec3(-0.5f, -0.3f, 0.65f)).get(vert.xyz);
		Ffp::addVertex(vert);
		Ffp::setColor(col_argb);

		vec3(model_view * vec3(0.5f, -0.3f, 0.65f)).get(vert.xyz);
		Ffp::addVertex(vert);
		Ffp::setColor(col_argb);


		Ffp::endLines();
		Ffp::disable();

	}
	render_target->disable();
	render_target->unbindColorTextures();
	RenderState::restoreState();
}
