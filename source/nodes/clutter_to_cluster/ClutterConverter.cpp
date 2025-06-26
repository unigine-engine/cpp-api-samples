#include "ClutterConverter.h"

#include <UnigineObjects.h>

REGISTER_COMPONENT(ClutterConverter);

using namespace Unigine;
using namespace Math;

void ClutterConverter::init()
{
	clutter_mesh = checked_ptr_cast<ObjectMeshClutter>(clutterMesh.get());
}

void ClutterConverter::convertToCluster()
{
	remove_cluster();

	cluster_mesh = convert_mesh(clutter_mesh);
	if (cluster_mesh)
	{
		is_converted = true;
		// move cluster to another parent
		cluster_mesh->setParent(clusterParent.get());
	}
}

void ClutterConverter::generateClutter()
{
	clutter_mesh->setSeed(Math::Random::getRandom().get());
}

void ClutterConverter::remove_cluster()
{
	if (!is_converted)
		return;

	cluster_mesh.deleteLater();
	is_converted = false;
}

ObjectMeshClusterPtr ClutterConverter::convert_mesh(const ObjectMeshClutterPtr &clutter)
{
	auto cluster = ObjectMeshCluster::create(clutter->getMeshPath());
	String name(clutter->getName());
	cluster->setName(name.append("_Cluster"));
	// copy the hierarchy
	cluster->setParent(clutter->getParent());
	// copy node transformation
	cluster->setWorldTransform(clutter->getWorldTransform());

	// copy necessary parameters for surfaces
	cluster->setVisibleDistance(clutter->getVisibleDistance());
	cluster->setFadeDistance(clutter->getFadeDistance());

	const int suf_num = clutter->getNumSurfaces();
	for (int suf_index = 0; suf_index < suf_num; ++suf_index)
	{
		cluster->setEnabled(clutter->isEnabled(suf_index), suf_index);
		cluster->setViewportMask(clutter->getViewportMask(suf_index), suf_index);
		cluster->setShadowMask(clutter->getShadowMask(suf_index), suf_index);
		cluster->setCastShadow(clutter->getCastShadow(suf_index), suf_index);
		cluster->setCastWorldShadow(clutter->getCastWorldShadow(suf_index), suf_index);
		cluster->setBakeToEnvProbe(clutter->getBakeToEnvProbe(suf_index), suf_index);
		cluster->setBakeToGI(clutter->getBakeToGI(suf_index), suf_index);
		cluster->setCastEnvProbeShadow(clutter->getCastEnvProbeShadow(suf_index), suf_index);
		cluster->setShadowMode(clutter->getShadowMode(suf_index), suf_index);
		cluster->setMinVisibleDistance(clutter->getMinVisibleDistance(suf_index), suf_index);
		cluster->setMaxVisibleDistance(clutter->getMaxVisibleDistance(suf_index), suf_index);
		cluster->setMinFadeDistance(clutter->getMinFadeDistance(suf_index), suf_index);
		cluster->setMaxFadeDistance(clutter->getMaxFadeDistance(suf_index), suf_index);
		cluster->setMinParent(clutter->getMinParent(suf_index), suf_index);
		cluster->setMaxParent(clutter->getMaxParent(suf_index), suf_index);
		cluster->setIntersection(clutter->getIntersection(suf_index), suf_index);
		cluster->setIntersectionMask(clutter->getIntersectionMask(suf_index), suf_index);
		cluster->setCollision(clutter->getCollision(suf_index), suf_index);
		cluster->setCollisionMask(clutter->getCollisionMask(suf_index), suf_index);
		cluster->setPhysicsIntersection(clutter->getPhysicsIntersection(suf_index), suf_index);
		cluster->setPhysicsIntersectionMask(clutter->getPhysicsIntersectionMask(suf_index),
			suf_index);
		cluster->setSoundOcclusion(clutter->getSoundOcclusion(suf_index), suf_index);
		cluster->setSoundOcclusionMask(clutter->getSoundOcclusionMask(suf_index), suf_index);
		cluster->setPhysicsFriction(clutter->getPhysicsFriction(suf_index), suf_index);
		cluster->setPhysicsRestitution(clutter->getPhysicsRestitution(suf_index), suf_index);
		cluster->setMaterial(clutter->getMaterial(suf_index), suf_index);
		cluster->setSurfaceProperty(clutter->getSurfaceProperty(suf_index), suf_index);
	}

	// copy transforms from the clutter to the cluster
	Vector<Mat4> transforms;
	clutter->createClutterTransforms();
	if (!clutter->getClutterWorldTransforms(transforms))
	{
		Log::warning("ClutterConverter::convert_mesh(): empty set of transforms\n");
		return ObjectMeshClusterPtr{};
	}
	cluster->createMeshes(transforms);
	return cluster;
}
