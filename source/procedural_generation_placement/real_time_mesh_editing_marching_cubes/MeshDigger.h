// Real-time terrain deformation using Marching Cubes algorithm.
// Allows digging and filling with mouse clicks. Voxel field is stored
// in a 3D texture; mesh generation runs asynchronously.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWorld.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class AsyncMarchingCubes;

// Handles user input and delegates mesh generation to AsyncMarchingCubes.
class MeshDigger : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MeshDigger, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Number of voxels along each axis of the 3D field
	PROP_PARAM(Int, field_size, 64, nullptr, "Number of marching cubes along one side of the field");
	// World-space size of each voxel cube
	PROP_PARAM(Float, marching_cube_size, 0.2f, nullptr, "Marching Cube edge length");
	// Brush size for dig/fill operations in voxel units
	PROP_PARAM(Float, digging_radius, 2.f);
	// Material applied to generated terrain mesh
	PROP_PARAM(Material, mat, "Ground material");

private:
	void init();
	void update();
	void shutdown();

private:
	// Handles voxel field and mesh generation
	AsyncMarchingCubes *marching_cubes;
	// Reference to generated mesh for intersection tests
	Unigine::ObjectPtr ground_object;
	// Cached inverse transform for world-to-local conversion
	Unigine::Math::Mat4 ground_itransform;
	// Reusable intersection result object
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();

	SampleDescriptionWindow samples_description_window;
};


// Generates terrain mesh from voxel field using Marching Cubes algorithm.
// Mesh updates run asynchronously to avoid frame stalls.
class AsyncMarchingCubes
{
public:
	AsyncMarchingCubes(int num_cubes, float cube_edge = 0.2f);
	~AsyncMarchingCubes() { destroy(); }

	void create(int num_cubes, float cube_edge = 0.2f);
	void destroy();

	void update();

	// Spherical brush for modifying voxel field
	struct BrushSphere
	{
		Unigine::Math::vec3 pos{};	 // Center position in voxel coordinates
		float radius{6};			 // Brush radius in voxels
		float k{2};					 // Strength: positive fills, negative digs
	};
	// Queues a brush operation for next mesh update
	void addBrush(const BrushSphere &a) { actions.append(a); }

	Unigine::ObjectPtr getObject() const { return object; }
	void setMaterial(const Unigine::MaterialPtr &mat) { material = mat; }

private:
	// Starts async mesh generation if not already running
	void run(bool force = false);
	// Generates initial heightmap-based voxel field using noise
	void create_field();
	// Generates triangles from voxel field using lookup tables
	void marching_cubes(Unigine::MeshPtr mesh) const;
	// Applies all queued brush operations to voxel field
	void brush_field();
	// Modifies voxel values within sphere radius
	void add_sphere(Unigine::Math::vec3 pos, float radius, float k);
	// Async callback: applies brushes and regenerates mesh
	void update_ram(Unigine::MeshPtr mesh);
	// Async callback: enables collision after mesh update
	void update_ram_done();

private:
	// Generated terrain mesh object
	Unigine::ObjectMeshStaticPtr object;
	// 3D texture storing voxel density values
	Unigine::ImagePtr field;

	int size{0};	  // Voxel grid size per axis
	int size2{0};	  // size * size (cached for indexing)
	float cube_edge_length{0.2f};

	// Perlin noise generator for initial terrain
	Unigine::Math::Noise noise;

	// Pending brush operations (main thread)
	Unigine::Vector<BrushSphere> actions;
	// Brush operations being processed (async thread)
	Unigine::Vector<BrushSphere> async_actions;

	Unigine::MaterialPtr material;

	// Prevents concurrent mesh generation
	Unigine::AtomicBool is_running{false};
	// Safety flag for destruction during async operation
	bool is_deleted = false;

	// Marching Cubes lookup tables
	static const short marching_cubes_edges[];
	static const char marching_cubes_triangles[];
	// Unit cube corner positions
	static const Unigine::Math::vec3 cell_0;
	static const Unigine::Math::vec3 cell_1;
	static const Unigine::Math::vec3 cell_2;
	static const Unigine::Math::vec3 cell_3;
	static const Unigine::Math::vec3 cell_4;
	static const Unigine::Math::vec3 cell_5;
	static const Unigine::Math::vec3 cell_6;
	static const Unigine::Math::vec3 cell_7;
};
