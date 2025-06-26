#include "LandscapeHeightPainter.h"

#include <UnigineMaterials.h>

REGISTER_COMPONENT(LandscapeHeightPainter);

using namespace Unigine;
using namespace Math;

void LandscapeHeightPainter::enable()
{
	if (callback_handle.isValid())
		return;

	Landscape::getEventTextureDraw().connect(callback_handle, this, &LandscapeHeightPainter::texture_draw_callback);
}

void LandscapeHeightPainter::disable()
{
	if (!callback_handle.isValid())
		return;

	callback_handle.disconnect();
}

void LandscapeHeightPainter::paintAt(const Unigine::Math::Vec3 & world_position)
{
	auto distance = (world_position - last_paint_position).length();
	if (distance < brush_spacing * brush_size)
		return;
	last_paint_position = world_position;


	Vec3 brush_local_position = landscape_layer_map->getIWorldTransform() * world_position;
	quat brush_world_rotation = quat(vec3_up, brush_angle);
	quat brush_local_rotation = brush_world_rotation * inverse(landscape_layer_map->getWorldRotation());

	float half_size = brush_size / 2.0f;
	Vec3 brush_local_corners[4] = {
		brush_local_position + brush_local_rotation * Vec3(-half_size, -half_size, 0.0),
		brush_local_position + brush_local_rotation * Vec3( half_size, -half_size, 0.0),
		brush_local_position + brush_local_rotation * Vec3(-half_size,  half_size, 0.0),
		brush_local_position + brush_local_rotation * Vec3( half_size,  half_size, 0.0)
	};
	auto brush_local_bbox_min = Vec2{
		min(min(brush_local_corners[0].x, brush_local_corners[1].x), min(brush_local_corners[2].x, brush_local_corners[3].x)),
		min(min(brush_local_corners[0].y, brush_local_corners[1].y), min(brush_local_corners[2].y, brush_local_corners[3].y))
	};
	auto brush_local_bbox_max = Vec2{
		max(max(brush_local_corners[0].x, brush_local_corners[1].x), max(brush_local_corners[2].x, brush_local_corners[3].x)),
		max(max(brush_local_corners[0].y, brush_local_corners[1].y), max(brush_local_corners[2].y, brush_local_corners[3].y))
	};


	auto pixels_per_unit = Vec2{ landscape_layer_map->getResolution() } / Vec2{ landscape_layer_map->getSize() };
	auto drawing_region_coord = ivec2{ pixels_per_unit * brush_local_bbox_min };
	auto drawing_region_resolution = ivec2{ pixels_per_unit * (brush_local_bbox_max - brush_local_bbox_min) };
	auto local_brush_angle = brush_local_rotation.getAngle(vec3_up);


	auto id = Landscape::generateOperationID();
	per_operation_draw_data.insert(id, DrawData{
		blend_mode,
		brush_texture,
		brush_mask,
		local_brush_angle,
		brush_opacity,
		brush_height,
		brush_height_scale
	});
	Landscape::asyncTextureDraw(id, landscape_layer_map->getGUID(), drawing_region_coord,
		drawing_region_resolution, Landscape::FLAGS_DATA_HEIGHT);
}

void LandscapeHeightPainter::init()
{
	auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("landscape_height_brush.basemat"));
	if (!guid.isValid())
	{
		Log::warning("LandscapeHeightPainter::init(): can not find \"landscape_height_brush.basemat\" material\n");
		return;
	}

	brush_material = Materials::findMaterialByFileGUID(guid)->inherit();
}

void LandscapeHeightPainter::texture_draw_callback(const UGUID &guid, int id, const LandscapeTexturesPtr &buffer, const ivec2 &coord, int data_mask)
{
	auto it = per_operation_draw_data.find(id);
	if (it == per_operation_draw_data.end())
		return;

	auto render_target = Render::getTemporaryRenderTarget();

	RenderState::saveState();
	RenderState::clearStates();

	render_target->bindUnorderedAccessTexture(0, buffer->getHeight());
	render_target->enable();
	{
		brush_material->setState("blend_mode", static_cast<int>(it->data.blend_mode));
		brush_material->setTexture("brush_texture", it->data.brush_texture);
		brush_material->setTexture("brush_mask", it->data.brush_mask);
		brush_material->setParameterFloat("brush_angle", it->data.brush_angle);
		brush_material->setParameterFloat("brush_opacity", it->data.brush_opacity);
		brush_material->setParameterFloat("brush_height", it->data.brush_height);
		brush_material->setParameterFloat("brush_height_scale", it->data.brush_height_scale);

		brush_material->renderScreen("landscape_height_brush");

	}
	render_target->disable();
	render_target->unbindUnorderedAccessTexture(0);

	RenderState::restoreState();

	Render::releaseTemporaryRenderTarget(render_target);

	per_operation_draw_data.erase(it);
}
