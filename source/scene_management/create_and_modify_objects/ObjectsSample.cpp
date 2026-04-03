// Demonstrates creation and manipulation of three different object types:
// ObjectMeshStatic (loaded from file), ObjectMeshDynamic (procedural tetrahedron),
// and ObjectParticles. Shows runtime vertex modification, rigid body attachment,
// and material parameter changes via the inherited material system.

#include <UnigineComponentSystem.h>
#include <UnigineFileSystem.h>
#include <UnigineMesh.h>
#include <UniginePhysics.h>
#include <UnigineVisualizer.h>
#include <UniginePrimitives.h>
#include <UnigineObjects.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

namespace
{
	// Edge length for the procedural tetrahedron geometry
	constexpr float tetrahedron_edge = 1.4f;
}

class ObjectsSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(ObjectsSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Create each object type to demonstrate different mesh creation approaches
		create_static_mesh();
		create_dynamic_mesh();
		create_particles_object();

		// Enable visualizer for debug rendering of surfaces and shapes
		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);
	}

	void update()
	{
		// Render debug visualization for surfaces and physics shapes
		visualize_objects();
	}

	void update_physics()
	{
		// Apply initial spin to the tetrahedron on the first physics frame
		if (first_frame)
		{
			if (BodyRigidPtr body = dynamic_mesh->getBodyRigid())
			{
				// Angular impulse is scaled by inertia tensor for consistent rotation speed
				body->addAngularImpulse(body->getInertia() * vec3(0.f, 0.f, .5f));
			}
		}

		first_frame = false;
	}

	void shutdown()
	{
		gui.shutdown();
		// Restore original visualizer state
		Visualizer::setEnabled(visualizer_enabled);
	}

	void create_static_mesh()
	{
		// ObjectMeshStatic: geometry is loaded from a mesh file and cannot be modified at runtime
		static_mesh = ObjectMeshStatic::create();

		// resolvePartialVirtualPath() finds full path from partial filename
		static_mesh->setMeshPath(FileSystem::resolvePartialVirtualPath("material_ball.mesh"));

		// Wildcard "*" assigns material to all surfaces in the mesh
		static_mesh->setMaterialFilePath(FileSystem::resolvePartialVirtualPath("material_ball/fbx/material_ball_mat.mat"), "*");

		{
			// Create physics body with a box shape matching the mesh bounds
			BoundBox bbox = static_mesh->getBoundBox();

			BodyRigidPtr body = BodyRigid::create();
			ShapeBoxPtr shape = ShapeBox::create();

			// Shape size matches mesh bounding box dimensions
			shape->setSize(bbox.getSize());
			// Shape offset positions it at the bounding box center
			body->addShape(shape, translate(bbox.getCenter()));

			// Attach body to the object for physics simulation
			static_mesh->setBody(body);
		}

		static_mesh->setWorldPosition(Vec3(-3.0f, 0.f, 1.1f));
		static_mesh->setWorldRotation(quat(0.f, 0.f, 270.f));
	}

	void create_dynamic_mesh()
	{
		// ObjectMeshDynamic: geometry is created and can be modified at runtime
		// Constructing a tetrahedron - simplest 3D shape with four vertices

		// USAGE_IMMUTABLE_INDICES: triangle connectivity won't change
		// USAGE_DYNAMIC_VERTEX: vertex positions will be modified at runtime
		dynamic_mesh = ObjectMeshDynamic::create(ObjectMeshDynamic::USAGE_IMMUTABLE_INDICES | ObjectMeshDynamic::USAGE_DYNAMIC_VERTEX);

		// Pre-allocate GPU buffers: 4 triangles (12 indices) and 4 vertices
		dynamic_mesh->allocateIndices(3 * 4);
		dynamic_mesh->allocateVertex(4);

		{
			// Tetrahedron vertex positions calculated from regular tetrahedron formulas
			// Vertices are positioned on the circumsphere centered at origin
			dynamic_mesh->addVertex(vec3(0.f, - sqrtf(8.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(- sqrtf(2.f / 3.f), sqrtf(2.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(+ sqrtf(2.f / 3.f), sqrtf(2.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(0.f, 0.f, 1.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
		}

		{
			// UV coordinates for texture mapping (vec4 contains UV0 and UV1)
			dynamic_mesh->setTexCoord(0, vec4(vec2(0.f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(1, vec4(vec2(.33f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(2, vec4(vec2(.66f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(3, vec4(vec2(.5f, 1.f), vec2_zero));
		}

		{
			// Define triangle connectivity via index buffer
			// Counter-clockwise winding order defines front-facing triangles
			// (Back faces are culled and not rendered)

			// Bottom face
			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(2);

			// Three side faces connecting base to apex
			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(3);

			dynamic_mesh->addIndex(3);
			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(2);

			dynamic_mesh->addIndex(2);
			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(3);

			// Group all geometry into a named surface for material assignment
			dynamic_mesh->addSurface("surface");
		}

		// Calculate tangent vectors for normal mapping (required after vertex changes)
		dynamic_mesh->updateTangents();
		// Recalculate bounding box for culling (required after vertex changes)
		dynamic_mesh->updateBounds();

		// Assign material by surface name rather than index
		dynamic_mesh->setMaterialFilePath(FileSystem::resolvePartialVirtualPath("glass_mat.mgraph"), "surface");

		{
			// Create physics body with sphere approximation of tetrahedron
			BodyRigidPtr body = BodyRigid::create(dynamic_mesh);
			// Circumsphere radius for the tetrahedron
			ShapeSpherePtr shape = ShapeSphere::create(body, sqrtf(3.f / 8.f) * tetrahedron_edge);

			// Disable gravity for floating effect
			body->setGravity(false);
			// Use explicit mass/inertia rather than calculating from shape
			body->setShapeBased(false);
			body->setInertia(mat3_identity * 60.f);
			body->setMass(60.f);
		}

		dynamic_mesh->setWorldPosition(Vec3(0.f, 3.f, 1.2f));
		dynamic_mesh->setWorldRotation(quat(0.f, 0.f, 180.f));
	}

	void create_particles_object()
	{
		// ObjectParticles: dynamic particle emitter for effects
		particles = ObjectParticles::create();

		// Enable particle collision with all objects
		particles->setCollisionEnabled(true);
		particles->setCollisionMask(~0);  // All bits set = collide with everything

		// Enable physics intersection for raycasts to detect particles
		particles->setPhysicsIntersectionEnabled(true);
		particles->setPhysicsIntersectionMask(~0);

		// Configure emission parameters
		particles->setEmitterEnabled(1);
		particles->setSpawnRate(5.0f);        // Particles per second
		particles->setLife(2.0f, 0.5f);       // Base lifetime with variance

		particles->setWorldPosition(Vec3(3.f, 0.f, 1.0f));
	}

	void visualize_objects()
	{
		// Calculate visualization colors (zero alpha = disabled)
		vec4 surface_color = visualize_surfaces ? vec4(vec3(1.f), .66f) : vec4_zero;
		vec4 shape_color = visualize_shapes ? vec4(.97f, .9f, .356f, 1.f) : vec4_zero;

		// Lambda to render all surfaces of an object with wireframe overlay
		auto render_surfaces = [](const ObjectPtr &object, const vec4 &color)
		{
			for (int i = 0; i < object->getNumSurfaces(); i += 1)
				Visualizer::renderObjectSurface(object, i, color);
		};

		// Lambda to render physics collision shapes attached to object's body
		auto render_shapes = [](const ObjectPtr &object, const vec4 &color)
		{
			BodyPtr body = object->getBody();
			if (body)
			{
				// Each body can have multiple collision shapes
				for (int i = 0; i < body->getNumShapes(); i += 1)
					body->getShape(i)->renderVisualizer(color);
			}
		};

		// Apply visualization to all created objects
		render_surfaces(dynamic_mesh, surface_color);
		render_surfaces(static_mesh, surface_color);
		render_surfaces(particles, surface_color);

		render_shapes(dynamic_mesh, shape_color);
		render_shapes(static_mesh, shape_color);
		render_shapes(particles, shape_color);
	}

	void set_tetrahedron_base_relative_height(float height)
	{
		// Deform tetrahedron by moving base vertices while keeping apex fixed
		// This demonstrates runtime vertex modification

		// Convert relative height (-1 to 1) to world units
		height = height * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f));
		// Calculate circumsphere radius for constraint
		float circumsphere_radius = tetrahedron_edge * sqrtf(3.f / 8.f);
		// Base vertices stay on circumsphere: solve for XY radius at given Z
		float base_radius = sqrtf(circumsphere_radius * circumsphere_radius - height * height);

		// Position three base vertices evenly around Z axis at computed radius
		dynamic_mesh->setVertex(0, rotateZ(0.f * 360.f) * vec3(0.f, -base_radius, height));
		dynamic_mesh->setVertex(1, rotateZ(1.f /3.f * 360.f) * vec3(0.f, -base_radius, height));
		dynamic_mesh->setVertex(2, rotateZ(2.f /3.f * 360.f) * vec3(0.f, -base_radius, height));

		// flushVertex() uploads modified vertex data to GPU
		// Required after setVertex() for changes to take effect visually
		dynamic_mesh->flushVertex();
	}

	// ========================================================================================

	struct SampleGui : public EventConnections
	{
		void init(ObjectsSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

			// Slider to modify material albedo color at runtime
			sample_description_window.addFloatParameter(
				"material ball albedo brightness",
				"",
				1.f,
				0.f,
				1.f,
				[sample](float value)
				{
					// getMaterialInherit() creates instance-specific material copy on first call
					for (int i = 0; i < sample->static_mesh->getNumSurfaces(); i += 1)
						sample->static_mesh->getMaterialInherit(i)->setParameterFloat4("albedo_color", vec4(vec3(value), 1.f));
				}
			);

			// Slider to deform tetrahedron geometry in real-time
			sample_description_window.addFloatParameter(
				"tetrahedron base height",
				"",
				-1.f / 3.f,
				-1.f,
				1.f,
				[sample](float value) {
					sample->set_tetrahedron_base_relative_height(value);
				}
			);

			// Slider to adjust particle emission rate
			sample_description_window.addFloatParameter(
				"particles spawn rate",
				"",
				5.f,
				0.f,
				50.f,
				[sample](float value) {
					sample->particles->setSpawnRate(value);
				}
			);

			// Toggle for surface wireframe visualization
			sample_description_window.addBoolParameter(
				"show object surfaces",
				"",
				true,
				[sample](bool value) {
					sample->visualize_surfaces = value;
				}
			);

			// Toggle for physics shape visualization
			sample_description_window.addBoolParameter(
				"show object body shapes",
				"",
				false,
				[sample](bool value) {
					sample->visualize_shapes = value;
				}
			);
		}

		void shutdown()
		{
			sample_description_window.shutdown();
		}

		ObjectsSample *sample = nullptr;
		SampleDescriptionWindow sample_description_window;
	};

	// ========================================================================================

	// Procedural mesh with modifiable vertex data
	ObjectMeshDynamicPtr dynamic_mesh;
	// File-based mesh with fixed geometry
	ObjectMeshStaticPtr static_mesh;
	// Particle effect emitter
	ObjectParticlesPtr particles;

	// Saved visualizer state for restoration on shutdown
	bool visualizer_enabled = false;
	// UI-controlled flags for debug visualization
	bool visualize_surfaces = true;
	bool visualize_shapes = false;
	// One-shot flag for initial physics impulse
	bool first_frame = true;

	SampleGui gui;
};

REGISTER_COMPONENT(ObjectsSample);
