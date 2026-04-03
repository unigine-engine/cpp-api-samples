// Converts terrain height values to albedo colors using a gradient texture.
// Height range is mapped to gradient coordinates for colorization.
// Operates on entire landscape layer map using async texture draw.

#include "LandscapeHeightToAlbedo.h"

#include <UnigineMaterials.h>

REGISTER_COMPONENT(LandscapeHeightToAlbedo);

using namespace Unigine;
using namespace Math;

// Texture draw callback is connected to landscape event system.
void LandscapeHeightToAlbedo::enable()
{
	// Duplicate connections are prevented
	if (callback_handle.isValid())
		return;

	Landscape::getEventTextureDraw().connect(callback_handle, this, &LandscapeHeightToAlbedo::texture_draw_callback);
}

// Texture draw callback is disconnected from landscape event system.
void LandscapeHeightToAlbedo::disable()
{
	// Disconnection is skipped if not connected
	if (!callback_handle.isValid())
		return;

	callback_handle.disconnect();
}

// Async height-to-albedo conversion is started for entire landscape.
void LandscapeHeightToAlbedo::run()
{
	// Unique operation ID is generated for tracking
	auto id = Landscape::generateOperationID();

	// Current parameters are captured for async callback
	per_operation_draw_data.insert(id, DrawData{
		gradient,
		min_height,
		max_height
	});

	// Full landscape resolution is requested with albedo and height data
	Landscape::asyncTextureDraw(id, landscape_layer_map->getGUID(), { 0, 0 },
		landscape_layer_map->getResolution(), Landscape::FLAGS_DATA_ALBEDO | Landscape::FLAGS_DATA_HEIGHT);
}

// Shader material is loaded and instanced for height-to-albedo conversion.
void LandscapeHeightToAlbedo::init()
{
	// Material GUID is resolved from partial virtual path
	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("landscape_height_to_albedo.basemat"));
	if (!guid.isValid())
	{
		Log::warning("LandscapeHeightToAlbedo::init(): can not find \"landscape_height_to_albedo.basemat\" material\n");
		return;
	}

	// Unique material instance is created for this component
	material = Materials::findMaterialByFileGUID(guid)->inherit();
}

// Height-to-albedo shader is executed when async draw buffer is ready.
void LandscapeHeightToAlbedo::texture_draw_callback(const Unigine::UGUID &guid, int id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask)
{
	// Operation is verified to belong to this component
	auto it = per_operation_draw_data.find(id);
	if (it == per_operation_draw_data.end())
		return;

	// Temporary render target is acquired for shader pass
	auto render_target = Render::getTemporaryRenderTarget();

	// Render state is preserved for restoration after shader execution
	RenderState::saveState();
	RenderState::clearStates();

	// Albedo buffer is bound as render target for output
	render_target->bindColorTexture(0, buffer->getAlbedo());
	render_target->enable();
	{
		// Shader inputs are configured with height data and gradient mapping
		material->setTexture("height", buffer->getHeight());
		material->setTexture("gradient", it->data.gradient);
		material->setParameterFloat("min_height", it->data.min_height);
		material->setParameterFloat("max_height", it->data.max_height);

		// Full-screen shader pass is executed
		material->renderScreen("landscape_height_to_albedo");

	}
	render_target->disable();
	render_target->unbindColorTexture(0);

	// Render state is restored to previous configuration
	RenderState::restoreState();

	// Temporary render target is returned to pool
	Render::releaseTemporaryRenderTarget(render_target);

	// Completed operation data is removed from tracking
	per_operation_draw_data.erase(it);
}
