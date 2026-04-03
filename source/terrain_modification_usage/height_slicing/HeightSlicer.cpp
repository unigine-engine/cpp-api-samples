// Performs GPU-based height slicing on landscape terrain data. Reads height from
// a source LandscapeLayerMap and writes modified height/albedo to a target layer.
// Uses async texture draw callbacks and a state machine for multi-frame processing.

#include "HeightSlicer.h"

#include <UnigineMaterials.h>


REGISTER_COMPONENT(HeightSlicer);

using namespace Unigine;
using namespace Math;

void HeightSlicer::enable()
{
	// Connect to the Landscape texture draw event
	// Callback is invoked when asyncTextureDraw completes
	Landscape::getEventTextureDraw().connect(this, &HeightSlicer::texture_draw_callback);
}

void HeightSlicer::disable()
{
	// Removes all event connections for this object
	disconnectAll();
}

void HeightSlicer::setSourceLmap(
	const Unigine::LandscapeLayerMapPtr &in_source_lmap)
{
	maps.clear();
	source_lmap = in_source_lmap;
	// Maps vector used by Landscape::render for batch processing
	maps.push_back(source_lmap);
}

void HeightSlicer::run()
{
	// Ignore if already processing
	if (state != State::IDLE)
		return;

	// Begin the multi-frame slicing operation
	state = State::BEGIN_FETCH_HEIGHT;
}

void HeightSlicer::init()
{
	// Resolve partial path to full GUID for material lookup
	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath(
		"landscape_height_slice.basemat"));

	if (guid.isValid())
		// inherit() creates a modifiable instance of the base material
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
			// Skip if source layer has no valid resolution
			if (source_lmap->getResolution().x == 0 || source_lmap->getResolution().y == 0)
				return;

			// Create intermediate buffer for landscape texture operations
			if (!buffers)
			{
				buffers = LandscapeTextures::create(source_lmap->getResolution());
			}

			// Create texture to store rendered height data
			if (!source_lmap_height)
			{
				source_lmap_height = Texture::create();
				// R32F = 32-bit float for high precision height values
				// Multiple flags enable: filtering, render target usage, compute access
				source_lmap_height->create2D(source_lmap->getResolution().x, source_lmap->getResolution().y,
					Texture::FORMAT_R32F,
					Texture::SAMPLER_FILTER_LINEAR | Texture::FORMAT_USAGE_RENDER | Texture::SAMPLER_FILTER_BILINEAR | Texture::FORMAT_USAGE_UNORDERED_ACCESS | Texture::SAMPLER_WRAP_MASK);
			}

			// Landscape::render returns true when rendering is complete
			// Mat4_identity = no transform, texel size from source layer
			if (!buffers_rendered && Landscape::render(maps, buffers, Mat4_identity, source_lmap->getTexelSize().x, 0))
			{
				// Copy height data from buffer to our texture
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
			// Queue async draw operation on target layer
			// Callback will be invoked when GPU is ready
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
	// Get a temporary render target from the pool
	auto render_target = Render::getTemporaryRenderTarget();

	// Save current render state to restore after our rendering
	RenderState::saveState();
	RenderState::clearStates();

	// Bind output textures: albedo to slot 0, height to slot 1
	render_target->bindColorTexture(0, buffer->getAlbedo());
	render_target->bindColorTexture(1, buffer->getHeight());

	render_target->enable();
	{
		// Set shader parameters for the slice material
		slice_material->setTexture("source_height", source_lmap_height);
		// Convert normalized slice_height to actual world units
		slice_material->setParameterFloat("slice_height", toFloat(slice_height * source_lmap->getExtremumHeight().y));
		// Pass height range for normalization in shader
		slice_material->setParameterFloat("min_height", toFloat(source_lmap->getExtremumHeight().x));
		slice_material->setParameterFloat("max_height", toFloat(source_lmap->getExtremumHeight().y));

		// Execute the fullscreen slice shader pass
		slice_material->renderScreen("landscape_height_slice");
	}
	render_target->disable();
	// Unbind textures in reverse order
	render_target->unbindColorTexture(1);
	render_target->unbindColorTexture(0);

	// Restore previous render state
	RenderState::restoreState();
	// Return render target to the pool for reuse
	Render::releaseTemporaryRenderTarget(render_target);

	// Operation complete, return to idle state
	state = State::IDLE;
}
