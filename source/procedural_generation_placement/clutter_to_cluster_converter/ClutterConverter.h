// Converts procedural ObjectMeshClutter to static ObjectMeshCluster. Clutter
// regenerates instances every frame based on seed; Cluster stores fixed transforms
// for better runtime performance when instance positions do not need to change.

#pragma once

#include <UnigineComponentSystem.h>

class ClutterConverter : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ClutterConverter, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	// Source clutter object whose instances will be converted
	PROP_PARAM(Node, clutterMesh);
	// Parent node for the created cluster (can differ from clutter's parent)
	PROP_PARAM(Node, clusterParent)

	// Creates a static cluster from current clutter instance positions
	void convertToCluster();
	// Randomizes clutter placement by assigning a new seed value
	void generateClutter();

private:
	// Tracks whether a cluster has been created to prevent duplicates
	bool is_converted = false;
	// Cached reference to the source clutter object
	Unigine::ObjectMeshClutterPtr clutter_mesh;
	// The resulting cluster after conversion
	Unigine::ObjectMeshClusterPtr cluster_mesh;

private:
	void init();
	// Destroys existing cluster before creating a new one
	void remove_cluster();
	// Performs the actual conversion: copies mesh, surfaces, and transforms
	Unigine::ObjectMeshClusterPtr convert_mesh(const Unigine::ObjectMeshClutterPtr &clutter);
};
