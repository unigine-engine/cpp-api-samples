#include "BodyCallbacks.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(BodyCallbacks)


void BodyCallbacks::init()
{
	// parameters validation
	auto file_path = mesh_file.get();
	if (String::isEmpty(file_path))
	{
		Log::error("BodyCallbacks::init(): Mesh File parameter is empty!\n");
		return;
	}

	frozen_mat = frozen_materal.get();
	if (!frozen_mat)
	{
		Log::error("BodyCallbacks::init(): Frozen Matreial parameter is empty!\n");
		return;
	}

	position_mat = position_materal.get();
	if (!position_mat)
	{
		Log::error("BodyCallbacks::init(): Position Matreial parameter is empty!\n");
		return;
	}

	// general Physics settings
	Physics::setFrozenLinearVelocity(0.1f);
	Physics::setFrozenAngularVelocity(0.1f);
	Physics::setNumIterations(4);

	// create object and physical body with a box shape
	ObjectMeshStaticPtr object = ObjectMeshStatic::create(file_path);
	BodyRigidPtr body = BodyRigid::create(object);
	ShapeBoxPtr shape = ShapeBox::create(body, vec3(1));
	object->setMaterial(position_mat, "*");

	// create tower
	for (int i = 0; i < tower_level; i++)
		for (int j = 0; j < tower_level - i; j++)
		{
			// clone created earlier object and calculate it's position in a tower
			auto cloned_object = static_ptr_cast<ObjectMeshStatic>(object->clone());
			cloned_object->setWorldTransform(
				translate(Vec3(0.0f, j - 0.5f * (tower_level - i) + 0.5f, i + 0.5f) * space));

			// add Frozen, Position and Contact callbacks to new object's body
			body = cloned_object->getBodyRigid();
			body->getEventFrozen().connect(body_connections, this, &BodyCallbacks::frozen_callback);
			body->getEventPosition().connect(body_connections, this,
				&BodyCallbacks::position_callback);
			body->getEventContactEnter().connect(body_connections, this,
				&BodyCallbacks::contact_callback);
			objects.append(cloned_object);
		}

	object.deleteLater();
}

void BodyCallbacks::shutdown()
{
	// remove all connections
	body_connections.disconnectAll();
	objects.clear();
}

void BodyCallbacks::frozen_callback(const Unigine::BodyPtr &body)
{
	body->getObject()->setMaterial(frozen_mat, "*");
}

void BodyCallbacks::position_callback(const Unigine::BodyPtr &body)
{
	body->getObject()->setMaterial(position_mat, "*");
}

void BodyCallbacks::contact_callback(const Unigine::BodyPtr &body)
{
	body->renderContacts();
}
