// Demonstrates physics body event callbacks: frozen, position, and contact.
// A tower of rigid bodies is created; each body changes material based on state.
// Frozen bodies turn gray (sleeping), moving bodies are highlighted,
// and contact points are visualized when bodies touch.

#include "BodyCallbacks.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(BodyCallbacks)

// Tower of rigid bodies is created with event callbacks attached.
void BodyCallbacks::init()
{
	// Mesh file parameter is validated
	auto file_path = mesh_file.get();
	if (String::isEmpty(file_path))
	{
		Log::error("BodyCallbacks::init(): Mesh File parameter is empty!\n");
		return;
	}

	// Frozen state material is retrieved and validated
	frozen_mat = frozen_materal.get();
	if (!frozen_mat)
	{
		Log::error("BodyCallbacks::init(): Frozen Matreial parameter is empty!\n");
		return;
	}

	// Moving state material is retrieved and validated
	position_mat = position_materal.get();
	if (!position_mat)
	{
		Log::error("BodyCallbacks::init(): Position Matreial parameter is empty!\n");
		return;
	}

	// Physics frozen thresholds are configured
	Physics::setFrozenLinearVelocity(0.1f);
	Physics::setFrozenAngularVelocity(0.1f);
	Physics::setNumIterations(4);

	// Template object is created with rigid body and box shape
	ObjectMeshStaticPtr object = ObjectMeshStatic::create(file_path);
	BodyRigidPtr body = BodyRigid::create(object);
	ShapeBoxPtr shape = ShapeBox::create(body, vec3(1));
	object->setMaterial(position_mat, "*");

	// Pyramid tower is constructed by cloning template
	for (int i = 0; i < tower_level; i++)
		for (int j = 0; j < tower_level - i; j++)
		{
			// Object is cloned and positioned in pyramid formation
			auto cloned_object = static_ptr_cast<ObjectMeshStatic>(object->clone());
			cloned_object->setWorldTransform(
				translate(Vec3(0.0f, j - 0.5f * (tower_level - i) + 0.5f, i + 0.5f) * space));

			// Event callbacks are connected to cloned body
			body = cloned_object->getBodyRigid();
			body->getEventFrozen().connect(body_connections, this, &BodyCallbacks::frozen_callback);
			body->getEventPosition().connect(body_connections, this,
				&BodyCallbacks::position_callback);
			body->getEventContactEnter().connect(body_connections, this,
				&BodyCallbacks::contact_callback);

			// Object is stored for cleanup
			objects.append(cloned_object);
		}

	// Template object is deleted
	object.deleteLater();
}

// Event connections are removed and objects are cleared.
void BodyCallbacks::shutdown()
{
	// All body event callbacks are disconnected
	body_connections.disconnectAll();

	// Object references are cleared
	objects.clear();
}

// Frozen material is applied when body enters sleeping state.
void BodyCallbacks::frozen_callback(const Unigine::BodyPtr &body)
{
	// Gray material is applied to indicate frozen state
	body->getObject()->setMaterial(frozen_mat, "*");
}

// Active material is applied when body is moving.
void BodyCallbacks::position_callback(const Unigine::BodyPtr &body)
{
	// Highlighted material is applied to indicate motion
	body->getObject()->setMaterial(position_mat, "*");
}

// Contact points are visualized when collision is detected.
void BodyCallbacks::contact_callback(const Unigine::BodyPtr &body)
{
	// Contact points are rendered for debugging
	body->renderContacts();
}
