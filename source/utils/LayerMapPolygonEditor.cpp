#include "LayerMapPolygonEditor.h"
#include <UnigineGame.h>
#include <UnigineEngine.h>
#include "GeometryGenerator.h"

using namespace Unigine;
using namespace Math;

LayerMapPolygonEditor::LayerMapPolygonEditor(Unigine::LandscapeLayerMapPtr in_layer_map, Unigine::MaterialPtr material)
	:layer_map(in_layer_map), render_material(material)
{
	if (!layer_map) {
		Unigine::Log::warning("LayerMapEditor::init layer map in unspecified \n");
		return;
	}
	if (!render_material.get())
	{
		Unigine::Log::warning("LayerMapEditor::init render material in unspecified \n");
		return;
	}
	Unigine::Landscape::getEventTextureDraw().connect(event_connection, this, &LayerMapPolygonEditor::drawCallback);
	Unigine::Engine::get()->getEventBeginUpdate().connect(event_connection, this, &LayerMapPolygonEditor::update);
}

void LayerMapPolygonEditor::clearMasks(int flags_data_mask)
{
	auto mapRez = layer_map->getResolution();

	if (mapRez.x == 0 || mapRez.y == 0)
	{
		Log::warning("MaskGenerator::generatePolygonOnLayerMapMasks : Invalid map resolution\n");
		return;
	}

	DrawData data = {};
	data.action_type = ACTION_TYPE::CLEAR;
	data.masks = convertMasksToFileMasks(flags_data_mask);
	convertChannelMasks(flags_data_mask, data);

	auto id = Landscape::generateOperationID();
	operation_draw_data.insert(id, data);
	Landscape::asyncTextureDraw(id, layer_map->getGUID(), ivec2{ 0,0 }, layer_map->getResolution(), data.masks);
}

void LayerMapPolygonEditor::generateMeshOnMask(const Unigine::Vector<Unigine::Math::Vec3>& points, int flags_data_mask)
{
	if (points.size() < 3)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : too few points\n");
		return;
	}
	auto mapRez = layer_map->getResolution();

	if (mapRez.x == 0 || mapRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : Invalid layer map resolution\n");
		return;
	}
	Vector<Vec3> vertices = Vector<Vec3>(points.size());
	WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

	auto pixelsPerUnit = Vec2{ layer_map->getResolution() } / Vec2{ layer_map->getSize() };
	auto drawingCoord = ivec2{ pixelsPerUnit * Vec2(bb.minimum - layer_map->getWorldPosition()) };
	auto drawingRez = ivec2{ pixelsPerUnit * Vec2(bb.getSize()) };

	if (drawingRez.x == 0 || drawingRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is too small\n");
		return;
	}
	if (drawingRez.x > 4096 || drawingRez.y > 4096)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is to big for Landscape::asyncTextureDraw\n");
		return;
	}
	for (int i = 0; i < points.size(); i++)
	{
		vertices[i] = points[i] - bb.getCenter();
	}

	MeshPtr mesh = GeometryGenerator::CreatePolygonMesh(vertices, true);
	auto id = Landscape::generateOperationID();

	auto data = DrawData();
	data.mesh = mesh;
	convertChannelMasks(flags_data_mask, data);
	data.masks = convertMasksToFileMasks(flags_data_mask);
	data.action_type = DRAW_MASK;

	operation_draw_data.insert(id, data);
	Landscape::asyncTextureDraw(id, layer_map->getGUID(), drawingCoord, drawingRez, data.masks);
}

void LayerMapPolygonEditor::levelHeightForMesh(const Unigine::Vector<Unigine::Math::Vec3>& points, double height)
{
	if (points.size() < 3)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : too few points\n");
		return;
	}
	auto mapRez = layer_map->getResolution();

	if (mapRez.x == 0 || mapRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : Invalid layer map resolution\n");
		return;
	}

	WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

	auto pixelsPerUnit = Vec2{ layer_map->getResolution() } / Vec2{ layer_map->getSize() };
	auto drawingCoord = ivec2{ pixelsPerUnit * Vec2(bb.minimum - layer_map->getWorldPosition()) };
	auto drawingRez = ivec2{ pixelsPerUnit * Vec2(bb.getSize()) };

	if (drawingRez.x == 0 || drawingRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is too small\n");
		return;
	}
	if (drawingRez.x > 4096 || drawingRez.y > 4096)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is to big for Landscape::asyncTextureDraw\n");
		return;
	}
	Vector<Vec3> vertices = Vector<Vec3>(points.size());
	for (int i = 0; i < points.size(); i++)
	{
		vertices[i] = points[i] - bb.getCenter();
	}

	MeshPtr mesh = GeometryGenerator::CreatePolygonMesh(vertices, true);
	auto id = Landscape::generateOperationID();
	auto data = DrawData();
	data.mesh = mesh;
	data.height_value = height;
	data.height_scale = layer_map->getHeightScale();
	data.action_type = LEVEL_HEIGHT;
	data.masks = Landscape::FLAGS_FILE_DATA_HEIGHT | Landscape::FLAGS_FILE_DATA_OPACITY_HEIGHT;

	operation_draw_data.insert(id, data);
	Landscape::asyncTextureDraw(id, layer_map->getGUID(), drawingCoord, drawingRez, data.masks);
}

void LayerMapPolygonEditor::lowerTerrain(const Unigine::Vector<Unigine::Math::Vec3>& points, float lowering_amount)
{
	if (points.size() < 3)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : too few points\n");
		return;
	}
	auto mapRez = layer_map->getResolution();

	if (mapRez.x == 0 || mapRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : Invalid layer map resolution\n");
		return;
	}

	WorldBoundBox bb = WorldBoundBox(points.get(), points.size());

	auto pixelsPerUnit = Vec2{ layer_map->getResolution() } / Vec2{ layer_map->getSize() };
	auto drawingCoord = ivec2{ pixelsPerUnit * Vec2(bb.minimum - layer_map->getWorldPosition()) };
	auto drawingRez = ivec2{ pixelsPerUnit * Vec2(bb.getSize()) };

	if (drawingRez.x == 0 || drawingRez.y == 0)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is too small\n");
		return;
	}

	if (drawingRez.x > 4096 || drawingRez.y > 4096)
	{
		Log::warning("LayerMapPolygonEditor::generateMeshOnMask : mesh is to big for Landscape::asyncTextureDraw\n");
		return;
	}

	Vector<Vec3> vertices = Vector<Vec3>(points.size());
	for (int i = 0; i < points.size(); i++)
	{
		vertices[i] = points[i] - bb.getCenter();
	}

	MeshPtr mesh = GeometryGenerator::CreatePolygonMesh(vertices, true);
	auto id = Landscape::generateOperationID();
	auto data = DrawData();
	data.mesh = mesh;
	data.height_value = lowering_amount;
	data.height_scale = layer_map->getHeightScale();
	data.action_type = LOWER_HEIGHT;
	data.masks = Landscape::FLAGS_FILE_DATA_HEIGHT;


	operation_draw_data.insert(id, data);
	Landscape::asyncTextureDraw(id, layer_map->getGUID(), drawingCoord, drawingRez, data.masks);
}

void LayerMapPolygonEditor::setClutter(Unigine::ObjectMeshClutterPtr in_clutter)
{
	clutter = in_clutter;
}

void LayerMapPolygonEditor::drawCallback(const Unigine::UGUID& guid, int id, const Unigine::LandscapeTexturesPtr& buffer, const Unigine::Math::ivec2& coord, int data_mask)
{
	// one callback instead of resubscribing correct functions each time
	auto it = operation_draw_data.find(id);
	if (it == operation_draw_data.end())
		return;
	DrawData& drawData = it->data;
	switch (drawData.action_type)
	{
	case DRAW_MASK: drawMasks(drawData, buffer); break;
	case LEVEL_HEIGHT: drawLevelHeight(drawData, buffer); break;
	case LOWER_HEIGHT: drawLowerHeight(drawData, buffer); break;
	case CLEAR: clearMasks(drawData, buffer); break;
	default: break;
	}

	operation_draw_data.erase(it);
	need_clutter_update = 5;//5 frame delay for clutter invalidation.
}

void LayerMapPolygonEditor::drawMasks(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer)
{
	if (!data.mesh)
	{
		Log::error("LayerMapPolygonEditor::drawMasks mesh to draw is unspecified");
		return;
	}
	if (!buffer)
	{
		Log::error("LayerMapPolygonEditor::drawMasks buffer is nullptr");
		return;
	}

	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(data.mesh);

	auto bb = data.mesh->getBoundBox();
	Mat4 mesh_transform = Mat4_identity;
	mat4 projection = Math::ortho(
		-bb.getSize().x / 2,
		bb.getSize().x / 2,
		bb.getSize().y / 2,// Inverted along the Y axis(top and bottom have switched places); textures coords are (0;0) at left top corner
		-bb.getSize().y / 2,
		0.1f, 1000.f);

	mat4 camera_transform = Math::setTo(bb.getCenter() + vec3_up * 10, bb.getCenter(), vec3_forward, Math::AXIS_NZ);

	vec4 transforms[3];
	mat4 local_transform = mat4(mesh_transform);
	mat4* transform_array = &local_transform;
	vec4 color = vec4_zero;
	Simd::mulMat4Mat4(transforms, inverse(camera_transform), (const mat4**)&transform_array, 1);
	for (int i = 0; i <= 4; ++i)
	{
		int mask_flag = Landscape::FLAGS_FILE_DATA_MASK_0 << i;
		if (data.masks & mask_flag)
		{
			auto texture = buffer->getMask(i); // 4 mask in 1 file. 1 color channel per mask
			color = vec4
			{
				data.channel_masks[i] & 1 ? 255.0f : 0.0f,
				data.channel_masks[i] & 2 ? 255.0f : 0.0f,
				data.channel_masks[i] & 4 ? 255.0f : 0.0f,
				data.channel_masks[i] & 8 ? 255.0f : 0.0f,
			};
			if (!texture)
			{
				Log::error("MaskGenerator::drawPolygonOnMask can't get mask from textures buffer\n");
			}
			else
			{
				auto renderTarget = Render::getTemporaryRenderTarget();
				{
					RenderState::saveState();
					RenderState::clearStates();
					renderTarget->bindColorTexture(0, texture);

					renderTarget->enable();
					{
						Renderer::setModelview(inverse((Mat4)camera_transform));
						Renderer::setOldModelview(inverse((Mat4)camera_transform));
						Renderer::setProjection(projection);
						Renderer::setOldProjection(projection);

						ShaderPtr shader = render_material->getShaderForce(draw_mask_pass);
						shader->setParameterArrayFloat4("s_transform", transforms, 3);
						render_material->setParameterFloat4("mask_color", color);

						auto pass = render_material->getRenderPass(draw_mask_pass);
						Renderer::setMaterial(pass, render_material);
						Renderer::setShaderParameters(pass, render_material, 0);

						meshRender->render(MeshRender::MODE_TRIANGLES);

					}

					renderTarget->unbindAll();
					renderTarget->disable();

				}
				RenderState::restoreState();
				Render::releaseTemporaryRenderTarget(renderTarget);
			}
		}
	}

	for (int i = 0; i <= 4; ++i)
	{
		int mask_flag = Landscape::FLAGS_FILE_DATA_OPACITY_MASK_0 << i;
		if (data.masks & mask_flag)
		{
			auto texture = buffer->getOpacityMask(i);

			color = vec4
			{
				data.channel_masks[i] & 1 ? 255.0f : 0.0f,
				data.channel_masks[i] & 2 ? 255.0f : 0.0f,
				data.channel_masks[i] & 4 ? 255.0f : 0.0f,
				data.channel_masks[i] & 8 ? 255.0f : 0.0f,
			};
			if (!texture)
			{
				Log::error("MaskGenerator::drawPolygonOnMask can't get mask from textures buffer\n");
			}
			else
			{
				auto renderTarget = Render::getTemporaryRenderTarget();
				{

					RenderState::saveState();
					RenderState::clearStates();
					renderTarget->bindColorTexture(0, texture);
					renderTarget->enable();
					{
						Renderer::setModelview(inverse((Mat4)camera_transform));
						Renderer::setOldModelview(inverse((Mat4)camera_transform));
						Renderer::setProjection(projection);
						Renderer::setOldProjection(projection);

						ShaderPtr shader = render_material->getShaderForce(draw_mask_pass);
						shader->setParameterArrayFloat4("s_transform", transforms, 3);
						render_material->setParameterFloat4("mask_color", color);

						auto pass = render_material->getRenderPass(draw_mask_pass);
						Renderer::setMaterial(pass, render_material);
						Renderer::setShaderParameters(pass, render_material, 0);

						meshRender->render(MeshRender::MODE_TRIANGLES);

					}

					renderTarget->unbindAll();
					renderTarget->disable();


				}
				RenderState::restoreState();
				Render::releaseTemporaryRenderTarget(renderTarget);
			}
		}
	}
}

void LayerMapPolygonEditor::drawLevelHeight(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer)
{
	if (!data.mesh)
	{
		Log::error("LayerMapPolygonEditor::drawMasks mesh to draw is unspecified");
		return;
	}
	if (!buffer)
	{
		Log::error("LayerMapPolygonEditor::drawMasks buffer is nullptr");
		return;
	}
	auto heightTexture = buffer->getHeight();
	auto opacityTexture = buffer->getOpacityHeight();

	if (!heightTexture || !opacityTexture)
	{
		Log::error("LayerMapPolygonEditor::drawMasks buffer is nullptr");
		return;
	}
	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(data.mesh);

	auto bb = data.mesh->getBoundBox();
	Mat4 mesh_transform = Mat4_identity;
	mat4 projection = Math::ortho(
		-bb.getSize().x / 2,
		bb.getSize().x / 2,
		bb.getSize().y / 2,// Inverted along the Y axis(top and bottom have switched places); textures coords are (0;0) at left top corner but layermap's (0;0) is down-bottom corner
		-bb.getSize().y / 2,
		0.1f, 1000.f);

	mat4 camera_transform = Math::setTo(bb.getCenter() + vec3_up * 10, bb.getCenter(), vec3_forward, Math::AXIS_NZ);

	vec4 transforms[3];
	mat4 local_transform = mat4(mesh_transform);
	mat4* transform_array = &local_transform;
	vec4 color = vec4_zero;
	Simd::mulMat4Mat4(transforms, inverse(camera_transform), (const mat4**)&transform_array, 1);
	auto renderTarget = Render::getTemporaryRenderTarget();
	{
		RenderState::saveState();
		RenderState::clearStates();
		renderTarget->bindUnorderedAccessTexture(0, heightTexture);
		renderTarget->bindUnorderedAccessTexture(1, opacityTexture);
		renderTarget->enable();
		{
			Renderer::setModelview(inverse((Mat4)camera_transform));
			Renderer::setOldModelview(inverse((Mat4)camera_transform));
			Renderer::setProjection(projection);
			Renderer::setOldProjection(projection);

			ShaderPtr shader = render_material->getShaderForce(level_height_pass);
			shader->setParameterArrayFloat4("s_transform", transforms, 3);
			render_material->setParameterFloat("height_value", data.height_value / data.height_scale);

			auto pass = render_material->getRenderPass(level_height_pass);
			Renderer::setMaterial(pass, render_material);
			Renderer::setShaderParameters(pass, render_material, 0);

			meshRender->render(MeshRender::MODE_TRIANGLES);
		}

		renderTarget->unbindAll();
		renderTarget->disable();
		RenderState::restoreState();
	}
	Render::releaseTemporaryRenderTarget(renderTarget);
}

void LayerMapPolygonEditor::drawLowerHeight(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer)
{
	if (!data.mesh)
	{
		Log::error("LayerMapPolygonEditor::drawMasks mesh to draw is unspecified");
		return;
	}
	if (!buffer)
	{
		Log::error("LayerMapPolygonEditor::drawMasks buffer is nullptr");
		return;
	}
	auto opacityTexture = buffer->getHeight();

	if (!opacityTexture)
	{
		Log::error("LayerMapPolygonEditor::drawMasks buffer is nullptr");
		return;
	}
	MeshRenderPtr meshRender = MeshRender::create();
	meshRender->load(data.mesh);

	auto bb = data.mesh->getBoundBox();
	Mat4 mesh_transform = Mat4_identity;
	mat4 projection = Math::ortho(
		-bb.getSize().x / 2,
		bb.getSize().x / 2,
		bb.getSize().y / 2,// Inverted along the Y axis(top and bottom have switched places); textures coords are (0;0) at left top corner but layermap's (0;0) is down-bottom corner
		-bb.getSize().y / 2,
		0.1f, 1000.f);

	mat4 camera_transform = Math::setTo(bb.getCenter() + vec3_up * 10, bb.getCenter(), vec3_forward, Math::AXIS_NZ);

	vec4 transforms[3];
	mat4 local_transform = mat4(mesh_transform);
	mat4* transform_array = &local_transform;
	vec4 color = vec4_zero;
	Simd::mulMat4Mat4(transforms, inverse(camera_transform), (const mat4**)&transform_array, 1);
	auto renderTarget = Render::getTemporaryRenderTarget();
	{
		RenderState::saveState();
		RenderState::clearStates();
		renderTarget->bindUnorderedAccessTexture(0, opacityTexture);
		renderTarget->enable();
		{
			Renderer::setModelview(inverse((Mat4)camera_transform));
			Renderer::setOldModelview(inverse((Mat4)camera_transform));
			Renderer::setProjection(projection);
			Renderer::setOldProjection(projection);

			ShaderPtr shader = render_material->getShaderForce(lower_height_pass);
			shader->setParameterArrayFloat4("s_transform", transforms, 3);
			render_material->setParameterFloat("lowering_value", data.height_value / data.height_scale);

			auto pass = render_material->getRenderPass(lower_height_pass);
			Renderer::setMaterial(pass, render_material);
			Renderer::setShaderParameters(pass, render_material, 0);

			meshRender->render(MeshRender::MODE_TRIANGLES);
		}

		renderTarget->unbindAll();
		renderTarget->disable();
		RenderState::restoreState();
	}

	Render::releaseTemporaryRenderTarget(renderTarget);
}

void LayerMapPolygonEditor::clearMasks(const DrawData& data, const Unigine::LandscapeTexturesPtr& buffer)
{
	if (!buffer)
		return;

	for (int i = 0; i <= 4; ++i)
	{
		int mask_flag = Landscape::FLAGS_FILE_DATA_MASK_0 << i;
		if (data.masks & mask_flag)
		{
			auto texture = buffer->getMask(i);
			if (!texture)
			{
				Log::error("MaskGenerator::drawPolygonOnMask can't get mask from textures buffer\n");
			}
			else
			{
				texture->clearBuffer(data.draw_color);
			}
		}
	}
	for (int i = 0; i <= 4; ++i)
	{
		int mask_flag = Landscape::FLAGS_FILE_DATA_OPACITY_MASK_0 << i;
		if (data.masks & mask_flag)
		{
			auto texture = buffer->getOpacityMask(i);
			if (!texture)
			{
				Log::error("MaskGenerator::drawPolygonOnMask can't get opacity mask from textures buffer\n");
			}
			else
			{
				texture->clearBuffer(data.draw_color);
			}
		}
	}

}

int LayerMapPolygonEditor::convertMasksToFileMasks(int flags_data_mask)
{
	int fileMasks = 0;
	if (flags_data_mask & Landscape::FLAGS_DATA_HEIGHT)
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_HEIGHT;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_HEIGHT;
	}

	if ((flags_data_mask & Landscape::FLAGS_DATA_MASK_0) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_1) ||
		(flags_data_mask & Landscape::FLAGS_DATA_MASK_2) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_3))
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_MASK_0;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_MASK_0;
	}

	if ((flags_data_mask & Landscape::FLAGS_DATA_MASK_4) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_5) ||
		(flags_data_mask & Landscape::FLAGS_DATA_MASK_6) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_7))
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_MASK_1;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_MASK_1;
	}

	if ((flags_data_mask & Landscape::FLAGS_DATA_MASK_8) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_9) ||
		(flags_data_mask & Landscape::FLAGS_DATA_MASK_10) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_11))
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_MASK_2;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_MASK_2;
	}

	if ((flags_data_mask & Landscape::FLAGS_DATA_MASK_12) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_13) ||
		(flags_data_mask & Landscape::FLAGS_DATA_MASK_14) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_15))
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_MASK_3;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_MASK_3;
	}

	if ((flags_data_mask & Landscape::FLAGS_DATA_MASK_16) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_17) ||
		(flags_data_mask & Landscape::FLAGS_DATA_MASK_18) || (flags_data_mask & Landscape::FLAGS_DATA_MASK_19))
	{
		fileMasks |= Landscape::FLAGS_FILE_DATA_MASK_4;
		fileMasks |= Landscape::FLAGS_FILE_DATA_OPACITY_MASK_4;
	}
	return fileMasks;
}

void LayerMapPolygonEditor::convertChannelMasks(const int flags_data_mask, DrawData& out)
{
	for (int idx = 0; idx < 20; ++idx)
	{
		int flag = 1 << (2 + idx);//FLAGS_DATA_MASK_0 = 1 << 2 
		if (flags_data_mask & flag)
		{
			int file = idx / 4; // 0..4
			int channel = idx % 4; // 0..3
			out.channel_masks[file] |= (1 << channel);
		}
	}
}

void LayerMapPolygonEditor::update()
{
	if (need_clutter_update == 0)
		clutter->invalidate();
	if (need_clutter_update >= 0)
		need_clutter_update--;
}
