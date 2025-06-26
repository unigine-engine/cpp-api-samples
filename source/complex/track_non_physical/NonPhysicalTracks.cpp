#include "NonPhysicalTracks.h"

REGISTER_COMPONENT(NonPhysicalTracks);

using namespace Unigine;
using namespace Math;

void NonPhysicalTracks::init()
{
	if (!track_node.get())
		Log::error("NonPysicalTracks::init(): can not get targetNode property\n");

	mesh_static = static_ptr_cast<ObjectMeshStatic>(track_node.get());
	if (!mesh_static)
		Log::error("NonPysicalTracks::init():TrackNode is not ObjectMeshStatic\n");

	spline.init(node, mesh_static);
	spline.setSpacing(spacing);

	cluster = ObjectMeshCluster::create(mesh_static->getMeshPath());
	for (int i = 0; i < cluster->getNumSurfaces(); i++)
	{
		cluster->setIntersection(false, i);
		cluster->setCollision(false, i);
		cluster->setPhysicsIntersection(false, i);
		cluster->setMaterial(mesh_static->getMaterial(i), i);
	}
	cluster->setParent(node);
	
	spline.setParentTransform(cluster->getWorldTransform());

	is_initialized = true;
}

void NonPhysicalTracks::update()
{
	if (!is_initialized)
		return;

	// update tank tracks offset
	spline.setOffset(pos_offset);
	spline.setParentTransform(cluster->getWorldTransform());
	spline.update();
	
	cluster->createMeshes(spline.getMeshTrasform());
}

void NonPhysicalTracks::shutdown()
{
	if (cluster)
		cluster.deleteLater();
}

void NonPhysicalTracks::setOffset(float offset)
{
	pos_offset += offset;
}
