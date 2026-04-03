// Renders a camera view to a texture each frame and applies it as albedo
// to an object. Useful for security cameras, rear-view mirrors, portals.

#include "CameraToTexture.h"

#include <UnigineTextures.h>
#include <UnigineViewport.h>

REGISTER_COMPONENT(CameraToTexture);

using namespace Unigine;
using namespace Math;

// Creates render texture and viewport, inherits material from attached object,
// and assigns texture as albedo with UV transform correction for platform flipping.
void CameraToTexture::init()
{
	player = checked_ptr_cast<Player>(player_camera.get());
	texture = Texture::create();
	texture->create2D(256, 256, Texture::FORMAT_RG11B10F,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
	viewport = Viewport::create();
	auto obj = checked_ptr_cast<Object>(node);
	auto mat = obj->getMaterial(0);
	mat = mat->inherit();
	mat->setTexture(mat->findTexture("albedo"), texture);
	mat->setParameterFloat4("uv_transform", vec4(-1.f, Render::isFlipped() ? -1.f : 1.f, 0.f, 0.f));
	obj->setMaterial(mat, 0);
}

// Renders camera view to texture each frame.
void CameraToTexture::update()
{
	viewport->renderTexture2D(player->getCamera(), texture);
}
