#include "ProceduralMeshApply.h"

#include <UnigineGame.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(ProceduralMeshApply);


void ProceduralMeshApply::init()
{
	mesh = Mesh::create();
	cluster = ObjectMeshCluster::create();

	// before changing mesh choose Procedural Mode:
	// - Disable - procedural mode is disabled
	// - Dynamic - fastest performance, stored in RAM and VRAM, not automatically unloaded from
	// memory.
	// - Blob - moderate performance, stored in RAM and VRAM, automatically unloaded from memory.
	// - File - slowest performance, all data stored on disk, automatically unloaded from memory.
	cluster->setMeshProceduralMode(ObjectMeshStatic::PROCEDURAL_MODE_DYNAMIC);
	cluster->setWorldPosition(Vec3(0.f, 0.f, 3.f));

	// create cluster transforms
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

void ProceduralMeshApply::update()
{
	// change mesh before applying
	update_mesh(mesh);

	// Apply new mesh. You can do it Force or Async.
	// Changing mesh_render_flag you can choose where to store MeshRender data: in RAM or VRAM
	// 0 - store everything in VRAM (default behavior)
	// USAGE_DYNAMIC_VERTEX - store vertices on RAM
	// USAGE_DYNAMIC_INDICES - store indices on RAM
	// USAGE_DYNAMIC_ALL - store both vertices and indices on RAM
	cluster->applyMoveMeshProceduralAsync(mesh, 0);
	Visualizer::renderObject(cluster, vec4_green);
}

void ProceduralMeshApply::shutdown()
{
	mesh.clear();
	cluster.deleteLater();

	Visualizer::setEnabled(false);
}

void ProceduralMeshApply::update_mesh(MeshPtr mesh)
{
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

	mesh->clear();
	mesh->addSphereSurface("sphere", radius, num_stacks, num_slices);
}
