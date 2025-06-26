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

class MyObject : public ObjectExternBase
{
public:
	// unique class ID
	static constexpr int id = 2;

	MyObject()
	{
		Log::message("MyObject::MyObject(): called\n");
	}

	MyObject(void *node): ObjectExternBase(node)
	{
		Log::message("MyObject::MyObject(void*): called\n");
	}

	~MyObject() override
	{
		Log::message("MyObject::~MyObject(): called\n");
	}

	int getClassID() override { return id; }

	int getNumSurfaces() override { return 1; }
	const char *getSurfaceName(int surface) override { return "surface"; }

	const Math::BoundBox &getBoundBox(int surface) override { return bbox; }
	const Math::BoundSphere &getBoundSphere(int surface) override { return bsphere; }

	const BoundBox &getBoundBox() override { return bbox; }
	const BoundSphere &getBoundSphere() override { return bsphere; }

	bool hasRender() override { return true; }

	void render(Render::PASS pass, int surface) override
	{
		// check render pass
		if (pass != Render::PASS_AMBIENT)
			return;

		// set object surface
		ObjectPtr object = getObject();
		Renderer::setShaderParameters(pass, object, surface, false);

		// object color
		ShaderPtr shader = RenderState::getShader();
		shader->setParameterFloat4("extern_color", color);
		shader->flushParameters();

		// object geometry
		Ffp::beginTriangles();

		vec3 half_size = bbox.getSize() * .5f;;

		Ffp::addVertex(-half_size.x, -half_size.y, -half_size.z);
		Ffp::addVertex(half_size.x, -half_size.y, -half_size.z);
		Ffp::addVertex(-half_size.x, half_size.y, -half_size.z);
		Ffp::addVertex(half_size.x, half_size.y, -half_size.z);
		Ffp::addVertex(-half_size.x, -half_size.y, half_size.z);
		Ffp::addVertex(half_size.x, -half_size.y, half_size.z);
		Ffp::addVertex(-half_size.x, half_size.y, half_size.z);
		Ffp::addVertex(half_size.x, half_size.y, half_size.z);

		Ffp::addIndices(0, 3, 1);
		Ffp::addIndices(3, 0, 2);
		Ffp::addIndices(6, 5, 7);
		Ffp::addIndices(5, 6, 4);
		Ffp::addIndices(2, 7, 3);
		Ffp::addIndices(7, 2, 6);
		Ffp::addIndices(1, 4, 0);
		Ffp::addIndices(4, 1, 5);
		Ffp::addIndices(3, 5, 1);
		Ffp::addIndices(5, 3, 7);
		Ffp::addIndices(0, 6, 2);
		Ffp::addIndices(6, 0, 4);

		Ffp::endTriangles();
	}

	// custom method
	void setColor(const vec4 &color)
	{
		this->color = color;
	}

private:
	BoundBox bbox = BoundBox(vec3(-.5f), vec3(.5f));
	BoundSphere bsphere = BoundSphere(vec3(0.f), 1.f);
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
		ObjectExternBase::addClassID<MyObject>(MyObject::id);

		if (true)
		{
			// create a new MyObject instance
			ObjectExternPtr object_extern = ObjectExtern::create(MyObject::id);
			ObjectExternBase *object_extern_base = object_extern->getObjectExtern();
			if (object_extern_base)
			{
				my_object = static_cast<MyObject *>(object_extern_base);
			}
		}

		else
		{
			// alternatively you can instantiate MyObject directly (you'd need to free it later!)
			my_object = new MyObject();
		}

		if (my_object)
		{
			// you can access the base Object methods via ObjectExternBase::getObject
			my_object->getObject()->setWorldPosition(Vec3(0.f, 0.f, 1.f));

			// assign a body to the object
			{
				BodyRigidPtr body = BodyRigid::create();
				ShapePtr shape = ShapeBox::create(my_object->getBoundBox().getSize());
				body->addShape(shape);
				my_object->getObject()->setBody(body);
			}

			// assign a material to our object's "surface"
			my_object->getObject()->setMaterialFilePath(joinPaths(getWorldRootPath(), "materials", "object_extern.basemat"), "surface");

			// ... and access the custom MyObject methods directly
			my_object->setColor(vec4_white);
		}

		visualizer_enabled = Visualizer::isEnabled();
		Visualizer::setEnabled(true);
	}

	void update()
	{
	}

	void shutdown()
	{
		Visualizer::setEnabled(visualizer_enabled);
	}

	// ========================================================================================

	bool visualizer_enabled = false;
	MyObject *my_object = nullptr;
};

REGISTER_COMPONENT(ObjectExternSample);
