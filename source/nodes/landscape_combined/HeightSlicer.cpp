#include "HeightSlicer.h"

#include <UnigineMaterials.h>


REGISTER_COMPONENT(HeightSlicer);

using namespace Unigine;
using namespace Math;

void HeightSlicer::enable()
{
	Landscape::getEventTextureDraw().connect(this, &HeightSlicer::texture_draw_callback);
}

void HeightSlicer::disable()
{
	disconnectAll();
}

void HeightSlicer::setSourceLmap(
	const Unigine::LandscapeLayerMapPtr &in_source_lmap)
{
	maps.clear();
	source_lmap = in_source_lmap;
	maps.push_back(source_lmap);
}

void HeightSlicer::run()
{
	if (state != State::IDLE)
		return;

	state = State::BEGIN_FETCH_HEIGHT;
}

void HeightSlicer::init()
{
	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath(
		"landscape_height_slice.basemat"));

	if (guid.isValid())
		slice_material = Materials::findMaterialByFileGUID(guid)->inherit();
	else
		Log::warning(
			"HeightSlicer::init(): can not find \"landscape_height_slice.basemat\""
			"material\n");
}

void HeightSlicer::update()
{
	switch (state)
	{
		case State::IDLE:
			break;
		case State::BEGIN_FETCH_HEIGHT:
		{
			if (source_lmap->getResolution().x == 0 || source_lmap->getResolution().y == 0)
				return;

			if (!buffers)
			{
				buffers = LandscapeTextures::create(source_lmap->getResolution());
			}

			if (!source_lmap_height)
			{
				source_lmap_height = Texture::create();
				source_lmap_height->create2D(source_lmap->getResolution().x, source_lmap->getResolution().y,
					Texture::FORMAT_R32F,
					Texture::SAMPLER_FILTER_LINEAR | Texture::FORMAT_USAGE_RENDER | Texture::SAMPLER_FILTER_BILINEAR | Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::SAMPLER_WRAP_MASK);
			}

			if (!buffers_rendered && Landscape::render(maps, buffers, Mat4_identity, source_lmap->getTexelSize().x, 0))
			{
				source_lmap_height->copy(buffers->getHeight());
				buffers_rendered = true;
			} else if (buffers_rendered)
			{
				state = State::BEGIN_DRAW;
			}
			break;
		}
		case State::BEGIN_DRAW:
		{
			state = State::DRAWING;
			Landscape::asyncTextureDraw(target_lmap->getGUID(), {0, 0},
				target_lmap->getResolution());
			break;
		}
		default:
			break;
	}
}

void HeightSlicer::texture_draw_callback(const Unigine::UGUID &guid, int id,
	const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask)
{
	auto render_target = Render::getTemporaryRenderTarget();

	RenderState::saveState();
	RenderState::clearStates();

	render_target->bindColorTexture(0, buffer->getAlbedo());
	render_target->bindColorTexture(1, buffer->getHeight());

	render_target->enable();
	{
		slice_material->setTexture("source_height", source_lmap_height);
		slice_material->setParameterFloat("slice_height", toFloat(slice_height * source_lmap->getExtremumHeight().y));
		slice_material->setParameterFloat("min_height", toFloat(source_lmap->getExtremumHeight().x));
		slice_material->setParameterFloat("max_height", toFloat(source_lmap->getExtremumHeight().y));

		slice_material->renderScreen("landscape_height_slice");
	}
	render_target->disable();
	render_target->unbindColorTexture(1);
	render_target->unbindColorTexture(0);

	RenderState::restoreState();
	Render::releaseTemporaryRenderTarget(render_target);

	state = State::IDLE;
}
