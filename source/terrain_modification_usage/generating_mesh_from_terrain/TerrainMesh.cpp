// Generates a dynamic mesh from terrain height data using async LandscapeFetch.
// Grid of sample positions is fetched asynchronously with configurable concurrency.
// Resulting mesh vertices contain terrain height and albedo color from landscape.

#include "TerrainMesh.h"

#include <UnigineVisualizer.h>
#include <UnigineMeshDynamic.h>
#include <UnigineMaterials.h>

REGISTER_COMPONENT(TerrainMesh);

using namespace Unigine;
using namespace Math;

// Grid of sample positions is generated; async fetchers are initialized and started.
void TerrainMesh::run()
{
	// Operation is only started when not already fetching
	if (state != State::IDLE)
		return;

	// Grid corners are calculated from node transform
	Mat4 transform = node->getWorldTransform();
	Vec3 bottom_left = transform * Vec3(-0.5f, -0.5f, 0.0f);

	// Translation is removed to get direction vectors only
	transform.setColumn3(3, Vec3(0.0f, 0.0f, 0.0f));
	Vec3 delta_x = transform * Vec3(1.0f / resolution.x, 0.0f, 0.0f);
	Vec3 delta_y = transform * Vec3(0.0f, 1.0f / resolution.y, 0.0f);

	// Sample positions are generated as uniform grid
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

	// Fetcher pool is resized to match concurrency limit
	while (fetchers.size() < min(max_fetchers, fetch_positions.size()))
	{
		Fetcher &f = fetchers.emplace_back();
		f.landscape_fetch = LandscapeFetch::create();
		f.landscape_fetch->setUsesHeight(true);
		f.landscape_fetch->setUsesAlbedo(true);
	}

	// Excess fetchers are removed if pool is too large
	while (fetchers.size() > min(max_fetchers, fetch_positions.size()))
		fetchers.removeLast();

	// Initial batch of async fetches is started
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

// Visualizations are rendered; completed fetches are processed and new ones are queued.
void TerrainMesh::update()
{
	// Bounding box is visualized when enabled
	if (draw_bounding_box)
		Visualizer::renderBox(vec3{1.0f, 1.0f, 800.0f}, node->getTransform(), vec4_white);

	// Wireframe is visualized when mesh exists and flag is set
	if (draw_wireframe && mesh_dynamic)
	{
		Visualizer::renderObject(mesh_dynamic, vec4_white);
	}

	// Fetch results are processed when operation is in progress
	if (state == State::FETCHING)
	{
		for (int i = 0; i < fetchers.size(); ++i)
		{
			// Completed fetchers have their results stored
			if (fetchers[i].landscape_fetch->isAsyncCompleted())
			{
				fishined_fetch_count += 1;

				// Height and albedo are stored in results array
				fetch_data[fetchers[i].out_data_index].height = fetchers[i].landscape_fetch->getHeight();
				fetch_data[fetchers[i].out_data_index].albedo = fetchers[i].landscape_fetch->getAlbedo();

				// Fetcher is reused for next position if any remain
				if (next_fetch_index < fetch_positions.size())
				{
					fetchers[i].landscape_fetch->fetchAsync(fetch_positions[next_fetch_index]);
					fetchers[i].out_data_index = next_fetch_index;

					next_fetch_index += 1;
				}
			}
		}

		// Mesh is generated when all fetches are complete
		if (fishined_fetch_count == fetch_positions.size())
		{
			generate_mesh();
			state = State::IDLE;
		}
	}
}

// Dynamic mesh is built from fetched height and color data.
void TerrainMesh::generate_mesh()
{
	// Mesh object is created on first generation
	if (!mesh_dynamic)
	{
		mesh_dynamic = ObjectMeshDynamic::create();
		mesh_dynamic->setMaterial(material_prop.get(), "*");
		mesh_dynamic->translate(Vec3(0.0f, 0.0f, 200.f));
	}

	// Previous mesh data is cleared; buffers are pre-allocated
	mesh_dynamic->clearVertex();
	mesh_dynamic->clearIndices();
	mesh_dynamic->allocateVertex((resolution.x + 1) * (resolution.y + 1));
	mesh_dynamic->allocateIndices(resolution.x * resolution.y * 6);

	// Vertices are added with position from fetch coordinates and height
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

	// Triangle indices are generated for grid; invalid heights are skipped
	for (int i = 0; i < resolution.y; ++i)
	{
		auto pitch = resolution.x + 1;
		auto offset = pitch * i;

		for (int j = 0; j < resolution.x; ++j)
		{
			// Quad corners are calculated
			const int a = offset + j + 1;
			const int b = offset + pitch + j;
			const int c = offset + j;
			const int d = b + 1;

			// First triangle is skipped if any vertex has invalid height
			if (mesh_dynamic->getVertex(a).z == INVALID_FETCH_HEIGHT || mesh_dynamic->getVertex(b).z == INVALID_FETCH_HEIGHT ||
				mesh_dynamic->getVertex(c).z == INVALID_FETCH_HEIGHT)
			{
				continue;
			}

			mesh_dynamic->addIndex(a);
			mesh_dynamic->addIndex(b);
			mesh_dynamic->addIndex(c);

			// Second triangle is skipped if fourth vertex has invalid height
			if (mesh_dynamic->getVertex(d).z == INVALID_FETCH_HEIGHT)
			{
				continue;
			}

			mesh_dynamic->addIndex(a);
			mesh_dynamic->addIndex(d);
			mesh_dynamic->addIndex(b);
		}
	}

	// Tangents and bounds are recalculated; data is flushed to GPU
	mesh_dynamic->updateTangents();
	mesh_dynamic->updateBounds();
	mesh_dynamic->flushVertex();
	mesh_dynamic->flushIndices();

	// Transform is reapplied to update internal matrices
	mesh_dynamic->setTransform(mesh_dynamic->getTransform());
}
