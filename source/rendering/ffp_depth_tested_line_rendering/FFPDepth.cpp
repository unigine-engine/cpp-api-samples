// FFP rendering with depth testing. Draws 3D lines using the scene depth
// buffer for proper occlusion. Demonstrates manual projection setup with
// reverse depth and blending for overlays that respect scene geometry.

#include "FFPDepth.h"

#include <UnigineFfp.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(FFPDepth)

// Callback is registered for end-visualizer event (after scene rendering).
void FFPDepth::init()
{
	Render::getEventEndVisualizer().connect(this, &FFPDepth::render_lines_callback);
}

// FFP lines are drawn with depth test so they occlude behind scene geometry.
void FFPDepth::render_lines_callback()
{
	RenderTargetPtr render_target = Renderer::getRenderTarget();
	TexturePtr texture = Renderer::getTextureColor();

	RenderState::saveState();
	RenderState::clearStates();

	// Depth buffer is already bound from scene pass
	render_target->bindColorTexture(0, texture);

	render_target->enable();
	{
		// GEQUAL for reverse depth; alpha blending for overlay transparency
		RenderState::setDepthFunc(RenderState::DEPTH_GEQUAL);
		RenderState::setBlendFunc(RenderState::BLEND_SRC_ALPHA, RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
		RenderState::flushStates();

		Ffp::enable();

		// White texture allows vertex color to show through unchanged
		RenderState::setTexture(RenderState::BIND_FRAGMENT, 0, Render::getWhite2DTexture());

		// Projection is adjusted for reverse depth and [0..1] clip space
		mat4 projection;
		const mat4 offset = translate(0.f, 0.f, 0.5f) * scale(1.f, 1.f, 0.5f);
		mul(projection, offset, reverseDepthProjection((Renderer::getProjectionWithoutTAA())));

		Ffp::setTransform(projection);

		auto model_view = Renderer::getModelview();

		const unsigned col_argb = 0xffff0000;	// Red line

		Ffp::beginLines();
		Ffp::addLines(1);

		Ffp::Vertex vert;

		// Line endpoints are transformed from world to camera space
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
