// Implements terrain excavation using object depth projection. A digging object's
// shape is projected onto the terrain surface, modifying height and mask data
// in the underlying LandscapeLayerMap using async texture draw operations.

#include "DiggingTool.h"

#include <Unigine.h>
#include <UnigineTextures.h>

REGISTER_COMPONENT(DiggingTool);

using namespace Unigine;
using namespace Math;

// Layer map and digging object references are obtained; material is created.
void DiggingTool::init()
{
	// Node references are validated and cast to required types
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

		// Excavation material is loaded and inherited for runtime modification
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

	// Async texture draw callback is connected; previous modifications are cleared
	{
		Landscape::getEventTextureDraw().connect(texture_draw_connection, this, &DiggingTool::on_texture_draw);
		reset_modifications();
	}

	// Mouse handling and GUI manipulators are initialized
	{
		Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);
		target_transform = digging_object->getWorldTransform();
		gui.init(this);
	}
}

// Digging object is interpolated; terrain is updated when object moves significantly.
void DiggingTool::update()
{
	update_digging_object();

	// C key resets all terrain modifications
	if (Input::isKeyDown(Input::KEY_C))
		reset_modifications();

	// Terrain update is only enqueued when position/rotation changed enough
	if (should_update_terrain())
		enqueue_terrain_update();

	gui.update();
}

// Event connection is removed; pending operations are cleaned up.
void DiggingTool::shutdown()
{
	texture_draw_connection.disconnect();
	reset_modifications();

	// Pending depth textures are destroyed
	for (auto &it: per_operation_draw_data)
		it.data.object_depth_texture->destroy();

	per_operation_draw_data.clear();

	gui.shutdown();
}

// =======================================================================================

// Digging object is smoothly interpolated toward target transform from manipulator.
void DiggingTool::update_digging_object()
{
	float ifps = Game::getIFps();

	// Bounding sphere is used to get square bounding box for consistent texture allocation
	digging_object_wbb = WorldBoundBox { digging_object->getWorldBoundSphere() };

	float position_lerp_rate = 5.f;
	float rotation_lerp_rate = 2.f;

	Mat4 transform = digging_object->getWorldTransform();

	Vec3 position = transform.getTranslate();

	// Position and rotation are interpolated toward target
	Vec3 new_position = lerp(position, target_transform.getTranslate(), position_lerp_rate * ifps);
	quat new_rotation = slerp(transform.getRotate(), target_transform.getRotate(), rotation_lerp_rate * ifps);

	{
		// Movement is clamped to prevent gaps in terrain modification
		// when object moves faster than terrain update rate

		float max_delta = position_spacing;

		Vec3 delta = new_position - position;
		Vec3 direction = normalize(delta);
		float magnitude = length(delta);

		magnitude = min(magnitude, max_delta);
		new_position = position + direction * magnitude;
	}

	digging_object->setWorldTransform(Mat4 { new_rotation, new_position });
}

// Returns true if object moved/rotated enough to warrant terrain update.
bool DiggingTool::should_update_terrain()
{
	// Terrain is only updated when visible difference would result
	// (defined by position_spacing and rotation_spacing thresholds)

	Vec3 position = digging_object->getWorldPosition();
	vec3 rotation = decomposeRotationXYZ(mat3(digging_object->getWorldTransform()));

	// Compare returns true if values are within tolerance (no update needed)
	if (compare(position, prev_position, position_spacing) && compare(rotation, prev_rotation, rotation_spacing))
		return false;

	prev_position = position;
	prev_rotation = rotation;

	return true;
}

// Depth texture is rendered and async terrain modification is queued.
void DiggingTool::enqueue_terrain_update()
{
	// Lambdas convert between world coordinates and landscape texel coordinates
	auto world_to_texel = [this](const Vec3 &world_pos, bool is_max) -> ivec2 {
		Vec2 coord = world_pos.xy / layer_map->getTexelSize();
		return ivec2(is_max ? ceil(coord) : floor(coord));
	};

	auto texel_to_world = [this](const ivec2 &texel_pos) -> Vec2 {
		return Vec2(texel_pos) * layer_map->getTexelSize();
	};

	TexturePtr object_depth_texture;

	// Bounding box is converted to texel coordinates
	ivec2 coord = world_to_texel(digging_object_wbb.minimum, true);
	ivec2 coord_max = world_to_texel(digging_object_wbb.maximum, false);
	ivec2 resolution = coord_max - coord;

	// Resolution is validated and aligned for texture reuse optimization
	{
		if (resolution.x <= 0 || resolution.y <= 0)
			return;

		// Alignment to 64 helps resource allocator reuse textures
		resolution = align(resolution, 64);
	}

	// Depth texture is created for storing object silhouette depth
	{
		object_depth_texture = Texture::create();

		// 32-bit float R-channel stores depth values
		int texture_format = Texture::FORMAT_R32F;

		int texture_flags = {
			Texture::SAMPLER_FILTER_LINEAR |
			Texture::SAMPLER_FILTER_BILINEAR |
			Texture::SAMPLER_WRAP_MASK |
			Texture::FORMAT_USAGE_RENDER |	// Required for render-to-texture
			0
		};

		object_depth_texture->create2D(resolution.x, resolution.y, texture_format, texture_flags);
	}

	// Object depth is rendered from above into texture
	{
		Vec3 position { texel_to_world(coord), digging_object_wbb.minimum.z };
		Vec3 size { texel_to_world(resolution), digging_object_wbb.getSize().z };

		object_depth.renderDepthTexture(object_depth_texture, digging_object, position, size, resolution);
	}

	// Async texture draw operation is queued for landscape system
	{
		int id = Landscape::generateOperationID();

		DrawData draw_data;
		draw_data.resolution = resolution;
		draw_data.object_depth_texture = object_depth_texture;

		per_operation_draw_data.append(id, draw_data);

		// Flags specify which terrain data layers to modify
		int flags_file_data = {
			Landscape::FLAGS_DATA_HEIGHT |		// Modify heightmap
			Landscape::FLAGS_FILE_DATA_MASK_4 |	// Modify mask 4 (example)
			0
		};

		Landscape::asyncTextureDraw(id, layer_map->getGUID(), coord, draw_data.resolution, flags_file_data);
	}
}

// =======================================================================================

// Callback invoked when async texture draw is ready; shader pass modifies terrain data.
void DiggingTool::on_texture_draw(const UGUID &guid, int operation_id, const LandscapeTexturesPtr &buffer, const ivec2 &coords, int data_mask)
{
	// Draw data for this operation is retrieved from pending map
	auto it = per_operation_draw_data.find(operation_id);
	if (it == per_operation_draw_data.end())
		return;

	// Terrain buffers and depth texture are bound to material for shader pass
	digging_tool_mat->setTexture("bind_height", buffer->getHeight());
	digging_tool_mat->setTexture("bind_mask", buffer->getMask(4));
	digging_tool_mat->setTexture("object_depth", it->data.object_depth_texture);

	// Shader expression modifies terrain height and mask based on object depth
	digging_tool_mat->runExpression("draw_object", it->data.resolution.x, it->data.resolution.y);

	// Textures are unbound after shader execution
	digging_tool_mat->setTexture("bind_height", nullptr);
	digging_tool_mat->setTexture("bind_mask", nullptr);
	digging_tool_mat->setTexture("object_depth", nullptr);

	// Depth texture is cleaned up after use
	it->data.object_depth_texture->destroy();
	per_operation_draw_data.remove(it);
}

// All terrain modifications from this tool are reverted to original state.
void DiggingTool::reset_modifications()
{
	Landscape::asyncResetModifications(layer_map->getGUID());
}

// =======================================================================================
// GUI IMPLEMENTATION
// =======================================================================================

// Translation and rotation manipulators are created for interactive object control.
void DiggingTool::Gui::init(DiggingTool *sample)
{
	this->sample = sample;

	auto gui = Unigine::Gui::getCurrent();

	// Lambda initializes common manipulator properties
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

	// Translator is active by default
	set_manipulator(translator);
}

// Manipulator is updated with camera matrices; keyboard switches tool mode.
void DiggingTool::Gui::update()
{
	PlayerPtr player = Game::getPlayer();

	if (!player || !active)
		return;

	// Camera matrices are passed to manipulator for correct 3D rendering
	active->setModelview(player->getCamera()->getModelview());
	active->setProjection(player->getProjection());
	active->setBasis(basis == LOCAL ? active->getTransform() : Mat4_identity);

	// Keyboard shortcuts switch between tools and coordinate systems
	if (!Input::isMouseGrab() && !Console::isActive())
	{
		if (Input::isKeyDown(Input::KEY_T))
			set_manipulator(translator);	// T = translate mode

		if (Input::isKeyDown(Input::KEY_R))
			set_manipulator(rotator);		// R = rotate mode

		if (Input::isKeyDown(Input::KEY_G))
			basis = (basis == LOCAL) ? WORLD : LOCAL;	// G = toggle local/world
	}

	// Target transform is updated from manipulator position
	sample->target_transform = active->getTransform();
}

// Active manipulator is switched; transform is preserved between tools.
void DiggingTool::Gui::set_manipulator(WidgetManipulatorPtr manipulator)
{
	WidgetManipulatorPtr previous = active;
	active = manipulator;

	// Transform is transferred from previous manipulator
	if (previous)
	{
		previous->setHidden(true);
		active->setTransform(previous->getTransform());
	}

	active->setHidden(false);
}

// Manipulator widgets are scheduled for deletion.
void DiggingTool::Gui::shutdown()
{
	translator.deleteLater();
	rotator.deleteLater();
}
