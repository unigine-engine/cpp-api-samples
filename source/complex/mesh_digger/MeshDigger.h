#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWorld.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class AsyncMarchingCubes;


class MeshDigger : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MeshDigger, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Int, field_size, 64, nullptr, "Number of marching cubes along one side of the field");
	PROP_PARAM(Float, marching_cube_size, 0.2f, nullptr, "Marching Cube edge length");
	PROP_PARAM(Float, digging_radius, 2.f);
	PROP_PARAM(Material, mat, "Ground material");

private:
	void init();
	void update();
	void shutdown();

private:
	AsyncMarchingCubes *marching_cubes;
	Unigine::ObjectPtr ground_object;
	Unigine::Math::Mat4 ground_itransform;

	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();

	SampleDescriptionWindow samples_description_window;
};


class AsyncMarchingCubes
{
public:
	AsyncMarchingCubes(int num_cubes, float cube_edge = 0.2f);
	~AsyncMarchingCubes() { destroy(); }

	void create(int num_cubes, float cube_edge = 0.2f);
	void destroy();

	void update();

	struct BrushSphere
	{
		Unigine::Math::vec3 pos{};
		float radius{6};
		float k{2};
	};
	void addBrush(const BrushSphere &a) { actions.append(a); }

	Unigine::ObjectPtr getObject() const { return object; }
	void setMaterial(const Unigine::MaterialPtr &mat) { material = mat; }

private:
	void run(bool force = false);
	void create_field();

	void marching_cubes(Unigine::MeshPtr mesh) const;

	// apply actions to the mesh
	void brush_field();
	// add a sphere to the current mesh
	void add_sphere(Unigine::Math::vec3 pos, float radius, float k);

	// update the mesh
	void update_ram(Unigine::MeshPtr mesh);
	void update_ram_done();

private:
	Unigine::ObjectMeshStaticPtr object;
	Unigine::ImagePtr field;

	int size{0};
	int size2{0};
	float cube_edge_length{0.2f};

	Unigine::Math::Noise noise;

	Unigine::Vector<BrushSphere> actions;
	Unigine::Vector<BrushSphere> async_actions;

	Unigine::MaterialPtr material;

	// use atomic flag to prevent intervention from another thread
	Unigine::AtomicBool is_running{false};

	// flag indicating whether the object is already deleted
	bool is_deleted = false;

	// Marching Cubes algorithm constants
	static const short marching_cubes_edges[];
	static const char marching_cubes_triangles[];

	static const Unigine::Math::vec3 cell_0;
	static const Unigine::Math::vec3 cell_1;
	static const Unigine::Math::vec3 cell_2;
	static const Unigine::Math::vec3 cell_3;
	static const Unigine::Math::vec3 cell_4;
	static const Unigine::Math::vec3 cell_5;
	static const Unigine::Math::vec3 cell_6;
	static const Unigine::Math::vec3 cell_7;
};
