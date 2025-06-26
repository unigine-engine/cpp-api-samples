#include "PhysicalTracks.h"

#include <UniginePhysics.h>

REGISTER_COMPONENT(PhysicalTracks);

using namespace Unigine;
using namespace Math;

void PhysicalTracks::init()
{
	if (!track_node.get())
		Log::error("PhysicalTracks::init(): can not get targetNode property\n");

	mesh_static = static_ptr_cast<ObjectMeshStatic>(track_node.get());
	if (!mesh_static)
		Log::error("PhysicalTracks::init():TrackNode is not ObjectMeshStatic\n");

//	find transforms to place track plates
	spline.init(node, mesh_static);
	spline.setSpacing(spacing);
	spline.setParentTransform(node->getWorldTransform());
	spline.update();
	auto mesh_transform = spline.getMeshTrasform();

	for (int i = 0; i < mesh_transform.size(); i++)
	{
		auto mesh = mesh_static->clone();
		mesh->setWorldTransform(mesh_transform[i]);
		mesh->setEnabled(true);
		nodes.append(mesh);
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		JointHingePtr joint = JointHinge::create(nodes[i]->getObjectBody(),
			nodes[(i + 1) % nodes.size()]->getObjectBody());

		joint->setWorldAxis(vec3(1.0f, 0.0f, 0.0f));

		joint->setLinearRestitution(0.6f);
		joint->setAngularRestitution(0.4f);
		joint->setLinearSoftness(0.2f);
		joint->setAngularSoftness(0.4f);
		joint->setAngularDamping(8.0f);

		// setting angular limits in degrees
		joint->setAngularLimitFrom(-50.0f);
		joint->setAngularLimitTo(50.0f);

		joint->setAngularSpring(8.0f);
		joint->setNumIterations(16);
	}
}

void PhysicalTracks::shutdown()
{
	for (int i = 0; i < nodes.size(); i++)
		nodes[i].deleteLater();
	nodes.clear();
}
