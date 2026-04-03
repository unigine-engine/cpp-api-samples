#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates applying procedural meshes to an ObjectMeshCluster.
// A sphere mesh is regenerated each frame with varying tessellation (stacks/slices),
// then applied asynchronously to all cluster instances. Uses PROCEDURAL_MODE_DYNAMIC
// for optimal performance when mesh data changes frequently.
class ProceduralMeshApply : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ProceduralMeshApply, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates applying a procedural sphere mesh to a cluster with dynamic tessellation");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();                              // Creates cluster and grid of instances
	void update();                            // Rebuilds mesh and applies to cluster
	void shutdown();                          // Releases mesh and cluster resources

	void update_mesh(Unigine::MeshPtr mesh);  // Regenerates sphere with current tessellation

private:
	Unigine::MeshPtr mesh;                    // Procedural mesh data
	Unigine::ObjectMeshClusterPtr cluster;    // Cluster displaying mesh instances

	// Base sphere radius
	float radius = 0.5f;

	// Limits for vertical subdivisions (stacks)
	const int max_num_stacks = 30;
	const int min_num_stacks = 2;

	// Current sphere tessellation
	int num_stacks = 2;
	int num_slices = 3;

	// Signals if we increase or decrease number of slices and stacks
	bool is_increasing = true;

	// Parameters for periodic change of sphere tessellation
	float change_rate = 0.1f;
	float current_time = 0.f;

	// Cluster layout
	const int size = 20;		// Number of cells along X and Y
	float offset = 1.f;			// Spacing between meshes
};
