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
		create_static_mesh();
		create_dynamic_mesh();
		create_particles_object();

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
		gui.init(this);
	}

	void update()
	{
		visualize_objects();
	}

	void update_physics()
	{
		if (first_frame)
		{
			if (BodyRigidPtr body = dynamic_mesh->getBodyRigid())
			{
				body->addAngularImpulse(body->getInertia() * vec3(0.f, 0.f, .5f));
			}
		}

		first_frame = false;
	}

	void shutdown()
	{
		gui.shutdown();
		Visualizer::setEnabled(visualizer_enabled);
	}

	void create_static_mesh()
	{
		static_mesh = ObjectMeshStatic::create();

		// set an existing mesh to be used in our static mesh
		static_mesh->setMeshPath(FileSystem::resolvePartialVirtualPath("material_ball.mesh"));

		// assign a material to all of the surfaces of the mesh using a wildcard pattern
		static_mesh->setMaterialFilePath(FileSystem::resolvePartialVirtualPath("material_ball/fbx/material_ball_mat.mat"), "*");

		{
			// add a rigid body to our static mesh object

			BoundBox bbox = static_mesh->getBoundBox();

			BodyRigidPtr body = BodyRigid::create();
			ShapeBoxPtr shape = ShapeBox::create();

			shape->setSize(bbox.getSize());
			body->addShape(shape, translate(bbox.getCenter()));

			static_mesh->setBody(body);
		}

		static_mesh->setWorldPosition(Vec3(-3.0f, 0.f, 1.1f));
		static_mesh->setWorldRotation(quat(0.f, 0.f, 270.f));
	}

	void create_dynamic_mesh()
	{
		// here we construct a tetrahedron - a simple shape with four vertices

		// in this sample we don't change the indices of the mesh (triangle configuration), but we do change the mesh vertices (their positions),
		// so we set the dynamic mesh flags correspondigly:
		dynamic_mesh = ObjectMeshDynamic::create(ObjectMeshDynamic::USAGE_IMMUTABLE_INDICES | ObjectMeshDynamic::USAGE_DYNAMIC_VERTEX);

		// allocate the memory for the vertices and indices
		dynamic_mesh->allocateIndices(3 * 4);
		dynamic_mesh->allocateVertex(4);

		{
			// add the four vertices to the mesh

			dynamic_mesh->addVertex(vec3(0.f, - sqrtf(8.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(- sqrtf(2.f / 3.f), sqrtf(2.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(+ sqrtf(2.f / 3.f), sqrtf(2.f / 9.f), -1.f / 3.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
			dynamic_mesh->addVertex(vec3(0.f, 0.f, 1.f) * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f)));
		}

		{
			// set up the texture coordinates for the vertices of our mesh, which are going to be used by materials

			dynamic_mesh->setTexCoord(0, vec4(vec2(0.f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(1, vec4(vec2(.33f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(2, vec4(vec2(.66f, 0.f), vec2_zero));
			dynamic_mesh->setTexCoord(3, vec4(vec2(.5f, 1.f), vec2_zero));
		}

		{
			// add the 12 indices corresponding to the 4 triangles of our mesh

			// note that the order of the three indices describing each triangle matters:
			// it encodes the information about which side of the triangle is "front" and which side is "back"
			// and is sometimes referred to as the "winding order" (you can learn more about it here: https://www.khronos.org/opengl/wiki/Face_Culling)

			// in Unigine counter-clockwise order is used to denote the "front" side of the triangle,
			// the "back" faces are culled and will not be rendered

			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(2);

			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(3);

			dynamic_mesh->addIndex(3);
			dynamic_mesh->addIndex(0);
			dynamic_mesh->addIndex(2);

			dynamic_mesh->addIndex(2);
			dynamic_mesh->addIndex(1);
			dynamic_mesh->addIndex(3);

			// save the resulting mesh as a surface with name "surface"
			dynamic_mesh->addSurface("surface");
		}

		// update the tangents and bounds after constructing the mesh
		dynamic_mesh->updateTangents();
		dynamic_mesh->updateBounds();

		// assign a material to the surface "surface" we've just created
		dynamic_mesh->setMaterialFilePath(FileSystem::resolvePartialVirtualPath("glass_mat.mgraph"), "surface");

		{
			// add a rigid body to our dynamic mesh object

			BodyRigidPtr body = BodyRigid::create(dynamic_mesh);
			ShapeSpherePtr shape = ShapeSphere::create(body, sqrtf(3.f / 8.f) * tetrahedron_edge);

			body->setGravity(false);
			body->setShapeBased(false);
			body->setInertia(mat3_identity * 60.f);
			body->setMass(60.f);
		}

		dynamic_mesh->setWorldPosition(Vec3(0.f, 3.f, 1.2f));
		dynamic_mesh->setWorldRotation(quat(0.f, 0.f, 180.f));
	}

	void create_particles_object()
	{
		particles = ObjectParticles::create();

		// here we create a particles object and configure some of its type-specific properties

		particles->setCollisionEnabled(true);
		particles->setCollisionMask(~0);

		particles->setPhysicsIntersectionEnabled(true);
		particles->setPhysicsIntersectionMask(~0);

		particles->setEmitterEnabled(1);
		particles->setSpawnRate(5.0f);
		particles->setLife(2.0f, 0.5f);

		particles->setWorldPosition(Vec3(3.f, 0.f, 1.0f));
	}

	void visualize_objects()
	{
		vec4 surface_color = visualize_surfaces ? vec4(vec3(1.f), .66f) : vec4_zero;
		vec4 shape_color = visualize_shapes ? vec4(.97f, .9f, .356f, 1.f) : vec4_zero;

		auto render_surfaces = [](const ObjectPtr &object, const vec4 &color)
		{
			for (int i = 0; i < object->getNumSurfaces(); i += 1)
				Visualizer::renderObjectSurface(object, i, color);
		};

		auto render_shapes = [](const ObjectPtr &object, const vec4 &color)
		{
			BodyPtr body = object->getBody();

			// check that the object has a body
			if (body)
			{
				for (int i = 0; i < body->getNumShapes(); i += 1)
					body->getShape(i)->renderVisualizer(color);
			}
		};

		render_surfaces(dynamic_mesh, surface_color);
		render_surfaces(static_mesh, surface_color);
		render_surfaces(particles, surface_color);

		render_shapes(dynamic_mesh, shape_color);
		render_shapes(static_mesh, shape_color);
		render_shapes(particles, shape_color);
	}

	void set_tetrahedron_base_relative_height(float height)
	{
		// here we change some of the vertex positions of our tetrahedron mesh

		height = height * tetrahedron_edge * 3.f / (2.f * sqrtf(6.f));
		float circumsphere_radius = tetrahedron_edge * sqrtf(3.f / 8.f);
		float base_radius = sqrtf(circumsphere_radius * circumsphere_radius - height * height);

		// update the vertex positions
		dynamic_mesh->setVertex(0, rotateZ(0.f * 360.f) * vec3(0.f, -base_radius, height));
		dynamic_mesh->setVertex(1, rotateZ(1.f /3.f * 360.f) * vec3(0.f, -base_radius, height));
		dynamic_mesh->setVertex(2, rotateZ(2.f /3.f * 360.f) * vec3(0.f, -base_radius, height));

		// call the flushVertex method after changing the vertex data so that it is updated on the GPU also
		dynamic_mesh->flushVertex();
	}

	// ========================================================================================

	struct SampleGui : public EventConnections
	{
		void init(ObjectsSample *sample)
		{
			this->sample = sample;

			sample_description_window.createWindow();

			sample_description_window.addFloatParameter(
				"material ball albedo brightness",
				"",
				1.f,
				0.f,
				1.f,
				[sample](float value)
				{
					for (int i = 0; i < sample->static_mesh->getNumSurfaces(); i += 1)
						sample->static_mesh->getMaterialInherit(i)->setParameterFloat4("albedo_color", vec4(vec3(value), 1.f));
				}
			);

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

			sample_description_window.addBoolParameter(
				"show object surfaces",
				"",
				true,
				[sample](bool value) {
					sample->visualize_surfaces = value;
				}
			);

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

	ObjectMeshDynamicPtr dynamic_mesh;
	ObjectMeshStaticPtr static_mesh;
	ObjectParticlesPtr particles;

	bool visualizer_enabled = false;
	bool visualize_surfaces = true;
	bool visualize_shapes = false;
	bool first_frame = true;

	SampleGui gui;
};

REGISTER_COMPONENT(ObjectsSample);
