#include "DiggingTool.h"

#include <Unigine.h>
#include <UnigineTextures.h>

REGISTER_COMPONENT(DiggingTool);

using namespace Unigine;
using namespace Math;

void DiggingTool::init()
{
	{
		layer_map = checked_ptr_cast<LandscapeLayerMap>(lmap_node.get());
		if (!layer_map)
		{
			Log::error("DiggingTool: can't cast node to lmap\n");
			return;
		}

		digging_object = checked_ptr_cast<Object>(digging_object_node.get());
		if (!digging_object)
		{
			Log::error("DiggingTool: digging object is null\n");
			return;
		}

		auto guid = FileSystem::getGUID(FileSystem::resolvePartialVirtualPath("digging_tool.basemat"));
		if (auto mat = Materials::findMaterialByFileGUID(guid))
		{
			digging_tool_mat = mat->inherit();
			if (!digging_tool_mat)
			{
				Log::error("DiggingTool: can't create digging tool material\n");
				return;
			}
		}
		else
		{
			Log::error("DiggingTool: can't create digging tool material\n");
			return;
		}
	}

	{
		Landscape::getEventTextureDraw().connect(texture_draw_connection, this, &DiggingTool::on_texture_draw);
		reset_modifications();
	}

	{
		Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);
		target_transform = digging_object->getWorldTransform();
		gui.init(this);
	}
}

void DiggingTool::update()
{
	update_digging_object();

	if (Input::isKeyDown(Input::KEY_C))
		reset_modifications();

	if (should_update_terrain())
		enqueue_terrain_update();

	gui.update();
}

void DiggingTool::shutdown()
{
	texture_draw_connection.disconnect();
	reset_modifications();

	for (auto &it: per_operation_draw_data)
		it.data.object_depth_texture->destroy();

	per_operation_draw_data.clear();

	gui.shutdown();
}

// =======================================================================================

void DiggingTool::update_digging_object()
{
	float ifps = Game::getIFps();

	// use bound sphere to get a square boundbox -- this will help a bit with
	// optimizing texture allocation later in `enqueue_terrain_update`
	digging_object_wbb = WorldBoundBox { digging_object->getWorldBoundSphere() };

	float position_lerp_rate = 5.f;
	float rotation_lerp_rate = 2.f;

	Mat4 transform = digging_object->getWorldTransform();

	Vec3 position = transform.getTranslate();

	Vec3 new_position = lerp(position, target_transform.getTranslate(), position_lerp_rate * ifps);
	quat new_rotation = slerp(transform.getRotate(), target_transform.getRotate(), rotation_lerp_rate * ifps);

	{
		// clamp the max delta the digging object can move in one frame
		// to avoid leaving odd patches of terrain unmodified

		// ideally this should be done using proper brush logic (with interpolation, etc.)
		// but that is beyond the scope of this sample

		float max_delta = position_spacing;

		Vec3 delta = new_position - position;
		Vec3 direction = normalize(delta);
		float magnitude = length(delta);

		magnitude = min(magnitude, max_delta);
		new_position = position + direction * magnitude;
	}

	digging_object->setWorldTransform(Mat4 { new_rotation, new_position });
}

bool DiggingTool::should_update_terrain()
{
	// to help performance we don't update terrain every frame, instead, it is done only when
	// it makes sense i.e. when the digging object has moved far enough to mark a visible difference on the terrain
	// (see defined by DiggingTool::*_spacing variables)

	Vec3 position = digging_object->getWorldPosition();
	vec3 rotation = decomposeRotationXYZ(mat3(digging_object->getWorldTransform()));

	if (compare(position, prev_position, position_spacing) && compare(rotation, prev_rotation, rotation_spacing))
		return false;

	prev_position = position;
	prev_rotation = rotation;

	return true;
}

void DiggingTool::enqueue_terrain_update()
{
	auto world_to_texel = [this](const Vec3 &world_pos, bool is_max) -> ivec2 {
		Vec2 coord = world_pos.xy / layer_map->getTexelSize();
		return ivec2(is_max ? ceil(coord) : floor(coord));
	};

	auto texel_to_world = [this](const ivec2 &texel_pos) -> Vec2 {
		return Vec2(texel_pos) * layer_map->getTexelSize();
	};

	TexturePtr object_depth_texture;

	ivec2 coord = world_to_texel(digging_object_wbb.minimum, true);
	ivec2 coord_max = world_to_texel(digging_object_wbb.maximum, false);
	ivec2 resolution = coord_max - coord;

	// check validity of the resolution and align it to optimize texture allocations:
	// when the texture resolution remains the same, the resource allocator will
	// be more likely to reuse it instead of creating new one every frame
	{
		if (resolution.x <= 0 || resolution.y <= 0)
			return;

		resolution = align(resolution, 64);
	}

	// create depth texture
	{
		object_depth_texture = Texture::create();

		// use 32-bit float R-channel format for storing the depth values
		int texture_format = Texture::FORMAT_R32F;

		int texture_flags = {
			Texture::SAMPLER_FILTER_LINEAR |
			Texture::SAMPLER_FILTER_BILINEAR |
			Texture::SAMPLER_WRAP_MASK |
			Texture::FORMAT_USAGE_RENDER | // flag for rendering to texture
			0
		};

		object_depth_texture->create2D(resolution.x, resolution.y, texture_format, texture_flags);
	}

	// render the digging object to depth texture
	{
		Vec3 position { texel_to_world(coord), digging_object_wbb.minimum.z };
		Vec3 size { texel_to_world(resolution), digging_object_wbb.getSize().z };

		object_depth.renderDepthTexture(object_depth_texture, digging_object, position, size, resolution);
	}

	// add texture draw operation to async queue
	{
		int id = Landscape::generateOperationID();

		DrawData draw_data;
		draw_data.resolution = resolution;
		draw_data.object_depth_texture = object_depth_texture;

		per_operation_draw_data.append(id, draw_data);

		int flags_file_data = {
			Landscape::FLAGS_DATA_HEIGHT | // height
			Landscape::FLAGS_FILE_DATA_MASK_4 | // mask 4 (just as an example)
			0
		};

		Landscape::asyncTextureDraw(id, layer_map->getGUID(), coord, draw_data.resolution, flags_file_data);
	}
}

// =======================================================================================

void DiggingTool::on_texture_draw(const UGUID &guid, int operation_id, const LandscapeTexturesPtr &buffer, const ivec2 &coords, int data_mask)
{
	// extract the draw data for this operation (object's depth texture and its resolution)

	auto it = per_operation_draw_data.find(operation_id);
	if (it == per_operation_draw_data.end())
		return;

	// bind the resources and update the terrain's height and masks in the "draw_object" pass of the `digging_tool.basemat` material

	digging_tool_mat->setTexture("bind_height", buffer->getHeight());
	digging_tool_mat->setTexture("bind_mask", buffer->getMask(4));
	digging_tool_mat->setTexture("object_depth", it->data.object_depth_texture);

	digging_tool_mat->runExpression("draw_object", it->data.resolution.x, it->data.resolution.y);

	digging_tool_mat->setTexture("bind_height", nullptr);
	digging_tool_mat->setTexture("bind_mask", nullptr);
	digging_tool_mat->setTexture("object_depth", nullptr);

	it->data.object_depth_texture->destroy();
	per_operation_draw_data.remove(it);
}

void DiggingTool::reset_modifications()
{
	Landscape::asyncResetModifications(layer_map->getGUID());
}

// =======================================================================================

void DiggingTool::Gui::init(DiggingTool *sample)
{
	this->sample = sample;

	auto gui = Unigine::Gui::getCurrent();

	auto init_manipulator = [this, gui](auto manipulator)
	{
		manipulator->setBasis(this->sample->target_transform);
		manipulator->setTransform(this->sample->target_transform);
		manipulator->setMask(WidgetManipulator::MASK_XYZ);
		manipulator->setRenderGui(gui);
		manipulator->setHidden(true);
		gui->addChild(manipulator);
	};

	translator = WidgetManipulatorTranslator::create(gui);
	rotator = WidgetManipulatorRotator::create(gui);

	init_manipulator(translator);
	init_manipulator(rotator);

	set_manipulator(translator);
}

void DiggingTool::Gui::update()
{
	PlayerPtr player = Game::getPlayer();

	if (!player || !active)
		return;

	active->setModelview(player->getCamera()->getModelview());
	active->setProjection(player->getProjection());
	active->setBasis(basis == LOCAL ? active->getTransform() : Mat4_identity);

	if (!Input::isMouseGrab() && !Console::isActive())
	{
		if (Input::isKeyDown(Input::KEY_T))
			set_manipulator(translator);

		if (Input::isKeyDown(Input::KEY_R))
			set_manipulator(rotator);

		if (Input::isKeyDown(Input::KEY_G))
			basis = (basis == LOCAL) ? WORLD : LOCAL;
	}

	sample->target_transform = active->getTransform();
}

void DiggingTool::Gui::set_manipulator(WidgetManipulatorPtr manipulator)
{
	WidgetManipulatorPtr previous = active;
	active = manipulator;

	if (previous)
	{
		previous->setHidden(true);
		active->setTransform(previous->getTransform());
	}

	active->setHidden(false);
}

void DiggingTool::Gui::shutdown()
{
	translator.deleteLater();
	rotator.deleteLater();
}
