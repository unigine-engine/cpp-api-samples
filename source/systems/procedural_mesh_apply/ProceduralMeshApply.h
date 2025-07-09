#pragma once

#include <UnigineComponentSystem.h>

class ProceduralMeshApply : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ProceduralMeshApply, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	void update_mesh(Unigine::MeshPtr mesh);

private:
	Unigine::MeshPtr mesh;
	Unigine::ObjectMeshClusterPtr cluster;

	float radius = 0.5f;

	const int max_num_stacks = 30;
	const int min_num_stacks = 2;

	int num_stacks = 2;
	int num_slices = 3;

	// signals if we increase or decrease number of slices and stacks
	bool is_increasing = true;

	// timer to change sphere parameters
	float change_rate = 0.1f;
	float current_time = 0.f;

	// x/y-size of cluster field
	const int size = 20;
	// offset between meshes
	float offset = 1.f;
};
