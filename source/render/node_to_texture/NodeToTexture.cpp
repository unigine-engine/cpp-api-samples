#include "NodeToTexture.h"

#include <UnigineTextures.h>

REGISTER_COMPONENT(NodeToTexture);

using namespace Unigine;
using namespace Math;

void NodeToTexture::init()
{
	player = checked_ptr_cast<Player>(player_camera.get());
	texture = Texture::create();
	texture->create2D(256, 256, Texture::FORMAT_RGBA8,
		Texture::SAMPLER_FILTER_LINEAR | Texture::SAMPLER_ANISOTROPY_16
			| Texture::FORMAT_USAGE_RENDER);
	viewport = Viewport::create();
	viewport->setNodeLightUsage(Viewport::USAGE_WORLD_LIGHT);
	viewport->setSkipFlags(
		Viewport::SKIP_TRANSPARENT | Viewport::SKIP_VELOCITY_BUFFER | Viewport::SKIP_POSTEFFECTS | Viewport::SKIP_VISUALIZER);
	viewport->setEnvironmentTexture(Render::getBlackCubeTexture());
	auto obj = checked_ptr_cast<ObjectMeshStatic>(object_view.get());
	auto mat = obj->getMaterial(0);
	mat = mat->inherit();
	mat->setTexture(mat->findTexture("albedo"), texture);
	mat->setParameterFloat4("uv_transform", vec4(-1.f, Render::isFlipped() ? -1.f : 1.f, 0, 0));
	obj->setMaterial(mat, 0);
}

void NodeToTexture::update()
{
	viewport->renderNodeTexture2D(player->getCamera(), node_to_render, texture);
}
