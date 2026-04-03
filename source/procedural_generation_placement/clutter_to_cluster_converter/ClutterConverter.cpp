// Converts procedural ObjectMeshClutter to static ObjectMeshCluster. Clutter
// regenerates instances every frame based on seed; Cluster stores fixed transforms
// for better runtime performance when instance positions do not need to change.

#include "ClutterConverter.h"

#include <UnigineObjects.h>

REGISTER_COMPONENT(ClutterConverter);

using namespace Unigine;
using namespace Math;

void ClutterConverter::init()
{
	// Cast the generic Node reference to ObjectMeshClutter for type-safe access
	clutter_mesh = checked_ptr_cast<ObjectMeshClutter>(clutterMesh.get());
}

void ClutterConverter::convertToCluster()
{
	// Remove any previously created cluster to avoid duplicates
	remove_cluster();

	cluster_mesh = convert_mesh(clutter_mesh);
	if (cluster_mesh)
	{
		is_converted = true;
		// Reparent cluster to specified node
		cluster_mesh->setParent(clusterParent.get());
	}
}

void ClutterConverter::generateClutter()
{
	// New seed triggers clutter to recalculate all instance positions
	clutter_mesh->setSeed(Math::Random::getRandom().get());
}

void ClutterConverter::remove_cluster()
{
	if (!is_converted)
		return;

	// deleteLater() safely removes the node at the end of the frame
	cluster_mesh.deleteLater();
	is_converted = false;
}

ObjectMeshClusterPtr ClutterConverter::convert_mesh(const ObjectMeshClutterPtr &clutter)
{
	// Create cluster using the same mesh asset as the source clutter
	auto cluster = ObjectMeshCluster::create(clutter->getMeshPath());
	String name(clutter->getName());
	cluster->setName(name.append("_Cluster"));

	// Preserve hierarchy: cluster takes clutter's place in the scene graph
	cluster->setParent(clutter->getParent());
	// Copy world transform to maintain spatial positioning
	cluster->setWorldTransform(clutter->getWorldTransform());

	// Copy LOD distance settings for consistent visibility behavior
	cluster->setVisibleDistance(clutter->getVisibleDistance());
	cluster->setFadeDistance(clutter->getFadeDistance());

	// Iterate through all surfaces to copy their rendering and physics settings
	const int suf_num = clutter->getNumSurfaces();
	for (int suf_index = 0; suf_index < suf_num; ++suf_index)
	{
		// Visibility and rendering settings
		cluster->setEnabled(clutter->isEnabled(suf_index), suf_index);
		cluster->setViewportMask(clutter->getViewportMask(suf_index), suf_index);

		// Shadow casting configuration
		cluster->setShadowMask(clutter->getShadowMask(suf_index), suf_index);
		cluster->setCastShadow(clutter->getCastShadow(suf_index), suf_index);
		cluster->setCastWorldShadow(clutter->getCastWorldShadow(suf_index), suf_index);

		// Global illumination and environment probe settings
		cluster->setBakeToEnvProbe(clutter->getBakeToEnvProbe(suf_index), suf_index);
		cluster->setBakeToGI(clutter->getBakeToGI(suf_index), suf_index);
		cluster->setCastEnvProbeShadow(clutter->getCastEnvProbeShadow(suf_index), suf_index);
		cluster->setShadowMode(clutter->getShadowMode(suf_index), suf_index);

		// Per-surface LOD distance overrides
		cluster->setMinVisibleDistance(clutter->getMinVisibleDistance(suf_index), suf_index);
		cluster->setMaxVisibleDistance(clutter->getMaxVisibleDistance(suf_index), suf_index);
		cluster->setMinFadeDistance(clutter->getMinFadeDistance(suf_index), suf_index);
		cluster->setMaxFadeDistance(clutter->getMaxFadeDistance(suf_index), suf_index);
		cluster->setMinParent(clutter->getMinParent(suf_index), suf_index);
		cluster->setMaxParent(clutter->getMaxParent(suf_index), suf_index);

		// Ray intersection settings for picking and visibility queries
		cluster->setIntersection(clutter->getIntersection(suf_index), suf_index);
		cluster->setIntersectionMask(clutter->getIntersectionMask(suf_index), suf_index);

		// Collision detection settings
		cluster->setCollision(clutter->getCollision(suf_index), suf_index);
		cluster->setCollisionMask(clutter->getCollisionMask(suf_index), suf_index);

		// Physics raycasting (separate from rendering intersection)
		cluster->setPhysicsIntersection(clutter->getPhysicsIntersection(suf_index), suf_index);
		cluster->setPhysicsIntersectionMask(clutter->getPhysicsIntersectionMask(suf_index),
			suf_index);

		// Sound occlusion for audio propagation
		cluster->setSoundOcclusion(clutter->getSoundOcclusion(suf_index), suf_index);
		cluster->setSoundOcclusionMask(clutter->getSoundOcclusionMask(suf_index), suf_index);

		// Physics material properties
		cluster->setPhysicsFriction(clutter->getPhysicsFriction(suf_index), suf_index);
		cluster->setPhysicsRestitution(clutter->getPhysicsRestitution(suf_index), suf_index);

		// Copy material and surface property references
		cluster->setMaterial(clutter->getMaterial(suf_index), suf_index);
		cluster->setSurfaceProperty(clutter->getSurfaceProperty(suf_index), suf_index);
	}

	// Extract current instance transforms from the procedural clutter
	Vector<Mat4> transforms;
	// Force clutter to compute its current instance positions
	clutter->createClutterTransforms();
	if (!clutter->getClutterWorldTransforms(transforms))
	{
		Log::warning("ClutterConverter::convert_mesh(): empty set of transforms\n");
		return ObjectMeshClusterPtr{};
	}

	// Create cluster instances from the extracted world-space transforms
	cluster->createMeshes(transforms);
	return cluster;
}
