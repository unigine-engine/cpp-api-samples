#include "TerrainMesh.h"

#include <UnigineVisualizer.h>
#include <UnigineMeshDynamic.h>
#include <UnigineMaterials.h>

REGISTER_COMPONENT(TerrainMesh);

using namespace Unigine;
using namespace Math;

void TerrainMesh::run()
{
	if (state != State::IDLE)
		return;

	Mat4 transform = node->getWorldTransform();
	Vec3 bottom_left = transform * Vec3(-0.5f, -0.5f, 0.0f);

	transform.setColumn3(3, Vec3(0.0f, 0.0f, 0.0f));
	Vec3 delta_x = transform * Vec3(1.0f / resolution.x, 0.0f, 0.0f);
	Vec3 delta_y = transform * Vec3(0.0f, 1.0f / resolution.y, 0.0f);


	fetch_positions.clear();
	for (int i = 0; i <= resolution.y; ++i)
	{
		for (int j = 0; j <= resolution.x; ++j)
		{
			Vec3 sample_point = bottom_left + delta_y * static_cast<Scalar>(i) + delta_x * static_cast<Scalar>(j);
			fetch_positions.push_back(sample_point.xy);
		}
	}

	fetch_data.resize(fetch_positions.size());

	while (fetchers.size() < min(max_fetchers, fetch_positions.size()))
	{
		Fetcher &f = fetchers.emplace_back();
		f.landscape_fetch = LandscapeFetch::create();
		f.landscape_fetch->setUsesHeight(true);
		f.landscape_fetch->setUsesAlbedo(true);
	}

	while (fetchers.size() > min(max_fetchers, fetch_positions.size()))
		fetchers.removeLast();


	next_fetch_index = 0;
	fishined_fetch_count = 0;
	for (int i = 0; i < fetchers.size(); ++i)
	{
		fetchers[i].landscape_fetch->fetchAsync(fetch_positions[next_fetch_index]);
		fetchers[i].out_data_index = next_fetch_index;



		next_fetch_index += 1;
	}

	state = State::FETCHING;
}

void TerrainMesh::update()
{
	if (draw_bounding_box)
		Visualizer::renderBox(vec3{1.0f, 1.0f, 800.0f}, node->getTransform(), vec4_white);

	if (draw_wireframe && mesh_dynamic)
	{
		Visualizer::renderObject(mesh_dynamic, vec4_white);
	}

	if (state == State::FETCHING)
	{
		for (int i = 0; i < fetchers.size(); ++i)
		{
			if (fetchers[i].landscape_fetch->isAsyncCompleted())
			{
				fishined_fetch_count += 1;

				fetch_data[fetchers[i].out_data_index].height = fetchers[i].landscape_fetch->getHeight();
				fetch_data[fetchers[i].out_data_index].albedo = fetchers[i].landscape_fetch->getAlbedo();

				if (next_fetch_index < fetch_positions.size())
				{
					fetchers[i].landscape_fetch->fetchAsync(fetch_positions[next_fetch_index]);
					fetchers[i].out_data_index = next_fetch_index;

					next_fetch_index += 1;
				}
			}
		}

		if (fishined_fetch_count == fetch_positions.size())
		{
			generate_mesh();
			state = State::IDLE;
		}
	}
}

void TerrainMesh::generate_mesh()
{
	if (!mesh_dynamic)
	{
		mesh_dynamic = ObjectMeshDynamic::create();
		mesh_dynamic->setMaterial(material_prop.get(), "*");
		mesh_dynamic->translate(Vec3(0.0f, 0.0f, 200.f));
	}

	mesh_dynamic->clearVertex();
	mesh_dynamic->clearIndices();
	mesh_dynamic->allocateVertex((resolution.x + 1) * (resolution.y + 1));
	mesh_dynamic->allocateIndices(resolution.x * resolution.y * 6);

	for (int i = 0; i <= resolution.y; ++i)
	{
		for (int j = 0; j <= resolution.x; ++j)
		{
			FetchData &data = fetch_data[(resolution.x + 1) * i + j];
			const auto &position = vec3(vec2(fetch_positions[(resolution.x + 1) * i + j]), data.height);

			mesh_dynamic->addVertex(position);
			mesh_dynamic->addColor(data.albedo);
		}
	}

	for (int i = 0; i < resolution.y; ++i)
	{
		auto pitch = resolution.x + 1;
		auto offset = pitch * i;

		for (int j = 0; j < resolution.x; ++j)
		{
			mesh_dynamic->addIndex(offset + j + 1);
			mesh_dynamic->addIndex(offset + pitch + j);
			mesh_dynamic->addIndex(offset + j);

			mesh_dynamic->addIndex(offset + j + 1);
			mesh_dynamic->addIndex(offset + pitch + j + 1);
			mesh_dynamic->addIndex(offset + pitch + j);
		}
	}

	mesh_dynamic->updateTangents();
	mesh_dynamic->updateBounds();
	mesh_dynamic->flushVertex();
	mesh_dynamic->flushIndices();

	mesh_dynamic->setTransform(mesh_dynamic->getTransform());
}
