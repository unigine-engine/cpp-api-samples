// Demonstrates joint breaking events. A bridge is constructed from rigid body
// segments connected by hinge joints with force/torque limits. When stress
// exceeds limits, joints break and trigger callbacks that change materials.

#include "JointCallbacks.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(JointCallbacks)

// Destructible bridge with breakable hinge joints is constructed.
void JointCallbacks::init()
{
	auto file_path = mesh_file.get();
	if (String::isEmpty(file_path))
	{
		Log::error("JointCallbacks::init(): Mesh File parameter is empty!\n");
		return;
	}

	broken_mat = broken_materal.get();
	if (!broken_mat)
	{
		Log::error("JointCallbacks::init(): Broken Matreial parameter is empty!\n");
		return;
	}

	joint_mat = joint_materal.get();
	if (!joint_mat)
	{
		Log::error("JointCallbacks::init(): Joint Matreial parameter is empty!\n");
		return;
	}

	// Lower thresholds allow bodies to freeze sooner, reducing CPU load
	Physics::setFrozenLinearVelocity(0.1f);
	Physics::setFrozenAngularVelocity(0.1f);

	ObjectMeshStaticPtr orig_object = ObjectMeshStatic::create(file_path);

	// Heavy weights with high density (80) will drop onto the bridge to test joint strength
	BodyRigidPtr body = BodyRigid::create(orig_object);
	ShapeBoxPtr shape = ShapeBox::create(body, vec3(1.f));
	shape->setDensity(80.0f);
	for (int i = 0; i < 3; i++)
	{
		auto object = static_ptr_cast<ObjectMeshStatic>(orig_object->clone());
		object->setWorldTransform(translate(Vec3(3.0f * (i - 1), 0.f, 12.0f)));
		objects.append(object);
	}

	// Template is reset for bridge segments (no body, different material)
	orig_object->setBody(nullptr);
	body.deleteLater();
	orig_object->setMaterial(joint_mat, "*");
	BodyPtr b0, b1;
	for (int i = 0; i < bridge_sections; i++)
	{
		auto object = static_ptr_cast<ObjectMeshStatic>(orig_object->clone());
		float pos = space * (i - (bridge_sections - 1) / 2.f);
		object->setWorldTransform(translate(Vec3(pos, 0.f, 8.f)));

		// End segments use BodyDummy (immovable anchors), middle ones use BodyRigid
		if (i == 0 || i == bridge_sections - 1)
			b1 = BodyDummy::create(object);
		else
			b1 = BodyRigid::create(object);
		ShapeBoxPtr shape2 = ShapeBox::create(b1, vec3(1.f));
		objects.append(object);

		// Hinge joints allow rotation only around X axis (like a real bridge chain)
		if (b0)
		{
			JointHingePtr joint = JointHinge::create(b0, b1, Vec3(pos - space, 0.f, 8.f),
				vec3_right);
			joint->setAngularDamping(8.f);
			joint->setNumIterations(4);
			joint->setLinearRestitution(0.02f);
			joint->setAngularRestitution(0.02f);
			// Force/torque limits define when joint breaks under stress
			joint->setMaxForce(1000.f);
			joint->setMaxTorque(16000.f);
			joint->getEventBroken().connect(joint_connections, this,
				&JointCallbacks::broken_callback);
		}

		b0 = b1;
	}

	orig_object.deleteLater();
}

void JointCallbacks::shutdown()
{
	joint_connections.disconnectAll();
	objects.clear();
}

// Visual feedback: both segments connected by broken joint change appearance.
void JointCallbacks::broken_callback(const Unigine::JointPtr &joint)
{
	joint->getBody0()->getObject()->setMaterial(broken_mat, "*");
	joint->getBody1()->getObject()->setMaterial(broken_mat, "*");
}
