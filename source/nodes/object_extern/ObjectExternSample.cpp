// Demonstrates ObjectExtern for creating custom renderable objects with user-defined
// geometry and rendering. Unlike NodeExtern, ObjectExtern supports surfaces, materials,
// and participates in the rendering pipeline. Custom objects can define their own
// geometry using Fixed Function Pipeline (Ffp) or custom shaders.

#include <UnigineComponentSystem.h>
#include <UnigineFfp.h>
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

// Custom renderable object inheriting from ObjectExternBase
class MyObject : public ObjectExternBase
{
public:
	// Unique identifier for this object type
	static constexpr int id = 2;

	// Default constructor for programmatic creation
	MyObject()
	{
		Log::message("MyObject::MyObject(): called\n");
	}

	// Factory constructor called by ObjectExtern::create()
	MyObject(void *node): ObjectExternBase(node)
	{
		Log::message("MyObject::MyObject(void*): called\n");
	}

	~MyObject() override
	{
		Log::message("MyObject::~MyObject(): called\n");
	}

	int getClassID() override { return id; }

	// Surface interface: defines how many materials can be assigned
	int getNumSurfaces() override { return 1; }
	const char *getSurfaceName(int surface) override { return "surface"; }

	// Per-surface bounding volumes for culling optimization
	const Math::BoundBox &getBoundBox(int surface) override { return bbox; }
	const Math::BoundSphere &getBoundSphere(int surface) override { return bsphere; }

	// Object-level bounding volumes (union of all surfaces)
	const BoundBox &getBoundBox() override { return bbox; }
	const BoundSphere &getBoundSphere() override { return bsphere; }

	// Enable rendering for this object
	bool hasRender() override { return true; }

	// Custom rendering callback called during the render pipeline
	void render(Render::PASS pass, int surface) override
	{
		// Only render during ambient pass (skip shadow, depth, etc.)
		if (pass != Render::PASS_AMBIENT)
			return;

		// Configure shader state for this object/surface
		ObjectPtr object = getObject();
		Renderer::setShaderParameters(pass, object, surface, false);

		// Set custom shader parameter (defined in material)
		ShaderPtr shader = RenderState::getShader();
		shader->setParameterFloat4("extern_color", color);
		shader->flushParameters();

		// Build cube geometry using Fixed Function Pipeline
		Ffp::beginTriangles();

		vec3 half_size = bbox.getSize() * .5f;

		// Define 8 cube vertices
		Ffp::addVertex(-half_size.x, -half_size.y, -half_size.z);
		Ffp::addVertex(half_size.x, -half_size.y, -half_size.z);
		Ffp::addVertex(-half_size.x, half_size.y, -half_size.z);
		Ffp::addVertex(half_size.x, half_size.y, -half_size.z);
		Ffp::addVertex(-half_size.x, -half_size.y, half_size.z);
		Ffp::addVertex(half_size.x, -half_size.y, half_size.z);
		Ffp::addVertex(-half_size.x, half_size.y, half_size.z);
		Ffp::addVertex(half_size.x, half_size.y, half_size.z);

		// Define 12 triangles (2 per face, 6 faces)
		// Bottom face
		Ffp::addIndices(0, 3, 1);
		Ffp::addIndices(3, 0, 2);
		// Top face
		Ffp::addIndices(6, 5, 7);
		Ffp::addIndices(5, 6, 4);
		// Back face
		Ffp::addIndices(2, 7, 3);
		Ffp::addIndices(7, 2, 6);
		// Front face
		Ffp::addIndices(1, 4, 0);
		Ffp::addIndices(4, 1, 5);
		// Right face
		Ffp::addIndices(3, 5, 1);
		Ffp::addIndices(5, 3, 7);
		// Left face
		Ffp::addIndices(0, 6, 2);
		Ffp::addIndices(6, 0, 4);

		Ffp::endTriangles();
	}

	// Custom method for setting render color
	void setColor(const vec4 &color)
	{
		this->color = color;
	}

private:
	// Unit cube bounding box
	BoundBox bbox = BoundBox(vec3(-.5f), vec3(.5f));
	// Bounding sphere for fast frustum culling
	BoundSphere bsphere = BoundSphere(vec3(0.f), 1.f);
	// Color passed to custom shader
	vec4 color = vec4_white;
};

class ObjectExternSample : public ComponentBase
{
public:
	COMPONENT_DEFINE(ObjectExternSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init()
	{
		// Register custom object type with factory
		ObjectExternBase::addClassID<MyObject>(MyObject::id);

		if (true)
		{
			// Create via engine factory (recommended)
			ObjectExternPtr object_extern = ObjectExtern::create(MyObject::id);
			ObjectExternBase *object_extern_base = object_extern->getObjectExtern();
			if (object_extern_base)
			{
				my_object = static_cast<MyObject *>(object_extern_base);
			}
		}

		else
		{
			// Direct instantiation (requires manual cleanup)
			my_object = new MyObject();
		}

		if (my_object)
		{
			// Access standard Object methods through getObject()
			my_object->getObject()->setWorldPosition(Vec3(0.f, 0.f, 1.f));

			// Attach physics body for physical interactions
			{
				BodyRigidPtr body = BodyRigid::create();
				// Create box shape matching object bounds
				ShapePtr shape = ShapeBox::create(my_object->getBoundBox().getSize());
				body->addShape(shape);
				my_object->getObject()->setBody(body);
			}

			// Assign custom material that defines the "extern_color" parameter
			my_object->getObject()->setMaterialFilePath(joinPaths(getWorldRootPath(), "materials", "object_extern.basemat"), "surface");

			// Use custom method to set render color
			my_object->setColor(vec4_white);
		}

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void update()
	{
		// Rendering is handled automatically through the render() callback
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	// Pointer to custom object (owned by ObjectExtern)
	MyObject *my_object = nullptr;
};

REGISTER_COMPONENT(ObjectExternSample);
