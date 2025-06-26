#include "LandscapeHeightToAlbedo.h"

#include <UnigineMaterials.h>

REGISTER_COMPONENT(LandscapeHeightToAlbedo);

using namespace Unigine;
using namespace Math;

void LandscapeHeightToAlbedo::enable()
{
	if (callback_handle.isValid())
		return;

	Landscape::getEventTextureDraw().connect(callback_handle, this, &LandscapeHeightToAlbedo::texture_draw_callback);
}

void LandscapeHeightToAlbedo::disable()
{
	if (!callback_handle.isValid())
		return;

	callback_handle.disconnect();
}

void LandscapeHeightToAlbedo::run()
{
	auto id = Landscape::generateOperationID();
	per_operation_draw_data.insert(id, DrawData{
		gradient,
		min_height,
		max_height
	});
	Landscape::asyncTextureDraw(id, landscape_layer_map->getGUID(), { 0, 0 },
		landscape_layer_map->getResolution(), Landscape::FLAGS_DATA_ALBEDO | Landscape::FLAGS_DATA_HEIGHT);
}

void LandscapeHeightToAlbedo::init()
{
	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("landscape_height_to_albedo.basemat"));
	if (!guid.isValid())
	{
		Log::warning("LandscapeHeightToAlbedo::init(): can not find \"landscape_height_to_albedo.basemat\" material\n");
		return;
	}

	material = Materials::findMaterialByFileGUID(guid)->inherit();
}

void LandscapeHeightToAlbedo::texture_draw_callback(const Unigine::UGUID &guid, int id, const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask)
{
	auto it = per_operation_draw_data.find(id);
	if (it == per_operation_draw_data.end())
		return;

	auto render_target = Render::getTemporaryRenderTarget();

	RenderState::saveState();
	RenderState::clearStates();

	render_target->bindColorTexture(0, buffer->getAlbedo());
	render_target->enable();
	{

		material->setTexture("height", buffer->getHeight());
		material->setTexture("gradient", it->data.gradient);
		material->setParameterFloat("min_height", it->data.min_height);
		material->setParameterFloat("max_height", it->data.max_height);

		material->renderScreen("landscape_height_to_albedo");

	}
	render_target->disable();
	render_target->unbindColorTexture(0);

	RenderState::restoreState();

	Render::releaseTemporaryRenderTarget(render_target);

	per_operation_draw_data.erase(it);
}
