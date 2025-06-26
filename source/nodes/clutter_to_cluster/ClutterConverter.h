#pragma once

#include <UnigineComponentSystem.h>

class ClutterConverter : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ClutterConverter, Unigine::ComponentBase);
	COMPONENT_INIT(init);

	PROP_PARAM(Node, clutterMesh);
	PROP_PARAM(Node, clusterParent)

	// create a cluster based on the current clutter
	void convertToCluster();
	// change clutter's seed
	void generateClutter();

private:
	// check if a cluster for the current clutter was created
	bool is_converted = false;
	Unigine::ObjectMeshClutterPtr clutter_mesh;
	Unigine::ObjectMeshClusterPtr cluster_mesh;

private:
	void init();
	void remove_cluster();
	Unigine::ObjectMeshClusterPtr convert_mesh(const Unigine::ObjectMeshClutterPtr &clutter);
};