// Demonstrates procedural mesh creation using Mesh::add*Surface() methods.
// Each primitive type (box, sphere, cylinder, capsule, prism, plane) is created
// by adding the corresponding surface to a Mesh, then wrapping it in ObjectMeshDynamic.

#include <UnigineComponentSystem.h>

using namespace Unigine;

class CreatePrimitivesSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(CreatePrimitivesSample, ComponentBase);
	COMPONENT_INIT(init);

private:
	void init()
	{
		// Create all primitive types spaced along the X axis
		create_box();
		create_sphere();
		create_cylinder();
		create_capsule();
		create_prism();
		create_plane();
	}

	void create_box()
	{
		// Mesh object holds geometry data before it becomes a scene node
		auto box_mesh = Mesh::create();

		// addBoxSurface() generates a 6-sided box with specified dimensions
		// Surface name is used for material assignment
		box_mesh->addBoxSurface("box_surface", Math::vec3(1.0f, 1.0f, 1.0f));

		// ObjectMeshDynamic copies the mesh data and becomes a renderable node
		auto box = ObjectMeshDynamic::create(box_mesh);
		box->setWorldPosition(Math::Vec3(-5.0f, 0.0f, 1.5f));

		// Clear the temporary Mesh; ObjectMeshDynamic has its own copy
		box_mesh.clear();
	}

	void create_sphere()
	{
		auto sphere_mesh = Mesh::create();
		// Parameters: radius, stacks (latitude divisions), slices (longitude divisions)
		sphere_mesh->addSphereSurface("sphere_surface", 0.5f, 16, 16);

		auto sphere = ObjectMeshDynamic::create(sphere_mesh);
		sphere->setWorldPosition(Math::Vec3(-3.0f, 0.0f, 1.5f));

		sphere_mesh.clear();
	}

	void create_cylinder()
	{
		auto cylinderMesh = Mesh::create();
		// Parameters: radius, height, stacks (height divisions), slices (radial divisions)
		cylinderMesh->addCylinderSurface("cylinder_surface", 0.5f, 1.0f, 16, 16);

		auto cylinder = ObjectMeshDynamic::create(cylinderMesh);
		cylinder->setWorldPosition(Math::Vec3(-1.0f, 0.0f, 1.5f));

		cylinderMesh.clear();
	}

	void create_capsule()
	{
		auto capsuleMesh = Mesh::create();
		// Capsule: cylinder with hemispherical caps at both ends
		// Parameters: radius, height (of cylindrical section), stacks, slices
		capsuleMesh->addCapsuleSurface("capsule_surface", 0.5f, 1.0f, 16, 16);

		auto capsule = ObjectMeshDynamic::create(capsuleMesh);
		capsule->setWorldPosition(Math::Vec3(1.0f, 0.0f, 1.5f));

		capsuleMesh.clear();
	}

	void create_prism()
	{
		auto prismMesh = Mesh::create();
		// Prism: tapered cylinder with configurable top/bottom radii
		// Parameters: bottom radius, height, top radius, number of sides
		prismMesh->addPrismSurface("prism_surface", 0.5f, 1.0f, 0.5f, 5);

		auto prism = ObjectMeshDynamic::create(prismMesh);
		prism->setWorldPosition(Math::Vec3(3.0f, 0.0f, 1.5f));

		prismMesh.clear();
	}

	void create_plane()
	{
		auto planeMesh = Mesh::create();
		// Parameters: width, height, number of subdivisions per axis
		// Single subdivision creates a simple quad
		planeMesh->addPlaneSurface("plane_surface", 1.0f, 1.0f, 1);

		auto plane = ObjectMeshDynamic::create(planeMesh);
		plane->setWorldPosition(Math::Vec3(5.0f, 0.0f, 1.5f));

		planeMesh.clear();
	}
};

REGISTER_COMPONENT(CreatePrimitivesSample);
