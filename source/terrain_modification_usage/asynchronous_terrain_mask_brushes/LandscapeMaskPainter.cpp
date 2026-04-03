// Implements terrain mask painting with configurable brush parameters.
// Brush strokes are applied to LandscapeLayerMap mask channels using async texture draw.
// Supports brush texture, opacity, rotation, spacing, and color customization.

#include "LandscapeMaskPainter.h"

#include <UnigineMaterials.h>

REGISTER_COMPONENT(LandscapeMaskPainter);

using namespace Unigine;
using namespace Math;

// Texture draw callback is connected to receive async paint completion events.
void LandscapeMaskPainter::enable()
{
	// Duplicate connection is prevented
	if (callback_handle.isValid())
	{
		return;
	}

	// Callback is registered for async texture draw completion
	Landscape::getEventTextureDraw().connect(callback_handle, this,
		&LandscapeMaskPainter::texture_draw_callback);
}

// Texture draw callback is disconnected to stop receiving paint events.
void LandscapeMaskPainter::disable()
{
	// Already disconnected state is handled
	if (!callback_handle.isValid())
	{
		return;
	}

	callback_handle.disconnect();
}

// Brush stroke is applied at world position; spacing controls stroke density.
void LandscapeMaskPainter::paintAt(const Unigine::Math::Vec3 &world_position)
{
	// Stroke is skipped if too close to previous stroke (based on spacing)
	auto distance = (world_position - last_paint_position).length();
	if (distance < brush_spacing * brush_size)
	{
		return;
	}

	last_paint_position = world_position;

	// World position is transformed to landscape local space
	Vec3 brush_local_position = landscape_layer_map->getIWorldTransform() * world_position;
	quat brush_world_rotation = quat(vec3_up, brush_angle);
	quat brush_local_rotation = brush_world_rotation
		* inverse(landscape_layer_map->getWorldRotation());
	float half_size = brush_size / 2.0f;

	// Four corners of rotated brush quad are calculated in local space
	Vec3 brush_local_corners[4]
		= {brush_local_position + brush_local_rotation * Vec3(-half_size, -half_size, 0.0),
			brush_local_position + brush_local_rotation * Vec3(half_size, -half_size, 0.0),
			brush_local_position + brush_local_rotation * Vec3(-half_size, half_size, 0.0),
			brush_local_position + brush_local_rotation * Vec3(half_size, half_size, 0.0)};

	// Axis-aligned bounding box is computed from rotated corners
	auto brush_local_bbox_min = Vec2{min(min(brush_local_corners[0].x, brush_local_corners[1].x),
										 min(brush_local_corners[2].x, brush_local_corners[3].x)),
		min(min(brush_local_corners[0].y, brush_local_corners[1].y),
			min(brush_local_corners[2].y, brush_local_corners[3].y))};
	auto brush_local_bbox_max = Vec2{max(max(brush_local_corners[0].x, brush_local_corners[1].x),
										 max(brush_local_corners[2].x, brush_local_corners[3].x)),
		max(max(brush_local_corners[0].y, brush_local_corners[1].y),
			max(brush_local_corners[2].y, brush_local_corners[3].y))};

	// Local coordinates are converted to pixel coordinates
	auto pixels_per_unit = Vec2{landscape_layer_map->getResolution()}
		/ Vec2{landscape_layer_map->getSize()};
	auto drawing_region_coord = ivec2{pixels_per_unit * brush_local_bbox_min};
	auto drawing_region_resolution = ivec2{
		pixels_per_unit * (brush_local_bbox_max - brush_local_bbox_min)};
	auto local_brush_angle = brush_local_rotation.getAngle(vec3_up);

	// Draw operation is queued with current brush parameters
	auto id = Landscape::generateOperationID();
	per_operation_draw_data.insert(id,
		DrawData{brush_texture, brush_mask, local_brush_angle, brush_opacity, brush_color});

	// Async texture draw is requested for mask channel 0 with opacity
	Landscape::asyncTextureDraw(id, landscape_layer_map->getGUID(), drawing_region_coord,
		drawing_region_resolution,
		static_cast<int>(Landscape::FLAGS_DATA_MASK_0)
			| static_cast<int>(Landscape::FLAGS_FILE_DATA_OPACITY_MASK_0));
}

// Brush material is loaded and inherited; clutter reference is validated.
void LandscapeMaskPainter::init()
{
	// Brush shader material is loaded from file
	auto guid = FileSystem::getGUID(
		FileSystem::resolvePartialVirtualPath("landscape_mask_brush.basemat"));
	if (!guid.isValid())
	{
		Log::error(
			"LandscapePainter::init(): can not find \"landscape_mask_brush.basemat\" material");
		return;
	}
	brush_material = Materials::findMaterialByFileGUID(guid)->inherit();

	// Clutter node reference is validated for mask-based invalidation
	if (!clutter_param.get())
	{
		Log::error("LandscapePainter::init(): attached node is not clutter");
		return;
	}

	clutter = checked_ptr_cast<ObjectMeshClutter>(clutter_param.get());
}

// Brush shader is executed when async texture draw buffer is ready.
void LandscapeMaskPainter::texture_draw_callback(const Unigine::UGUID &guid, int id,
	const Unigine::LandscapeTexturesPtr &buffer, const Unigine::Math::ivec2 &coord, int data_mask)
{
	// Draw data for this operation is retrieved from pending map
	auto it = per_operation_draw_data.find(id);
	if (it == per_operation_draw_data.end())
	{
		return;
	}

	// Temporary render target is obtained for shader execution
	auto render_target = Render::getTemporaryRenderTarget();

	// Render state is saved to restore after shader pass
	RenderState::saveState();
	RenderState::clearStates();

	// Mask and opacity textures are bound as unordered access for compute shader
	render_target->bindUnorderedAccessTexture(0, buffer->getMask(0));
	render_target->bindUnorderedAccessTexture(1, buffer->getOpacityMask(0));
	render_target->enable();
	{
		// Brush parameters are passed to shader material
		brush_material->setTexture("brush_texture", it->data.brush_texture);
		brush_material->setTexture("brush_mask", it->data.brush_mask);
		brush_material->setParameterFloat("brush_angle", it->data.brush_angle);
		brush_material->setParameterFloat("brush_opacity", it->data.brush_opacity);
		brush_material->setParameterFloat4("brush_color", it->data.brush_color);

		// Full-screen brush shader pass is executed
		brush_material->renderScreen("landscape_mask_brush");
	}
	render_target->disable();
	render_target->unbindUnorderedAccessTexture(0);
	render_target->unbindUnorderedAccessTexture(1);

	// Previous render state is restored
	RenderState::restoreState();

	// Temporary resources are released; operation data is cleaned up
	Render::releaseTemporaryRenderTarget(render_target);
	per_operation_draw_data.erase(it);

	// Clutter is invalidated to regenerate instances based on new mask
	if (clutter_param.get())
	{
		clutter->invalidate();
	}
}
