// Applies procedurally generated meshes to an ObjectMeshCluster using async updates.
// Demonstrates dynamic sphere tessellation with PROCEDURAL_MODE_DYNAMIC for meshes
// that change frequently. Cluster instances share the same procedural mesh geometry.

#include "ProceduralMeshApply.h"

#include <UnigineGame.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ProceduralMeshApply);

// Cluster is created with dynamic procedural mode and grid of instances is populated.
void ProceduralMeshApply::init()
{
	mesh = Mesh::create();
	cluster = ObjectMeshCluster::create();

	// Choose a procedural mode for the cluster:
	//		Disable - procedural mode is off
	//		Dynamic - highest performance; stored in RAM and VRAM; not auto-unloaded
	//		Blob    - medium performance; stored in RAM/VRAM; auto-unloaded when inactive
	//		File    - lowest performance; stored on disk; auto-unloaded when inactive
	cluster->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC);
	cluster->setWorldPosition(Vec3(0.f, 0.f, 3.f));

	// Create transforms for a grid of instances
	Vector<Mat4> transforms;
	float field_offset = (1.f + offset) * size / 2.f;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			transforms.append(
				translate(Vec3(x + x * offset - field_offset, y + y * offset - field_offset, 1.5)));
		}
	}
	cluster->appendMeshes(transforms);

	Visualizer::setEnabled(true);
}

// Mesh is regenerated and applied asynchronously each frame.
void ProceduralMeshApply::update()
{
	// Rebuild the sphere mesh before applying it to the cluster
	update_mesh(mesh);

	// Apply new mesh. You can do it Force or Async.
	// MeshRender storage flags (second argument) control where mesh data is kept:
	//		0						- default, keep data in VRAM
	//		USAGE_DYNAMIC_VERTEX	- store vertices on RAM
	//		USAGE_DYNAMIC_INDICES	- store indices on RAM
	//		USAGE_DYNAMIC_ALL		- store both vertices and indices on RAM

	cluster->applyMoveMeshProceduralAsync(mesh, 0);
	Visualizer::renderObject(cluster, vec4_green);
}

// Mesh and cluster resources are released.
void ProceduralMeshApply::shutdown()
{
	mesh.clear();
	cluster.deleteLater();

	Visualizer::setEnabled(false);
}

// Sphere mesh is regenerated with oscillating tessellation values.
void ProceduralMeshApply::update_mesh(MeshPtr mesh)
{
	// Periodically change sphere tessellation (stacks/slices)
	current_time += Game::getIFps();

	if (current_time > change_rate)
	{
		current_time = 0.f;

		num_slices = is_increasing ? num_slices + 1 : num_slices - 1;
		num_stacks = is_increasing ? num_stacks + 1 : num_stacks - 1;

		if (num_stacks == max_num_stacks)
			is_increasing = false;

		if (num_stacks <= min_num_stacks)
		{
			is_increasing = true;
			num_stacks = min_num_stacks;
			num_slices = num_stacks + 1;
		}
	}

	// Create a sphere surface with the current parameters
	mesh->clear();
	mesh->addSphereSurface("sphere", radius, num_stacks, num_slices);
}
