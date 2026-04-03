// Moving projectile that damages Pawn on collision. Uses updateSyncThread() for
// thread-safe transform updates and update() for game logic. Bounding box query
// detects collisions; VectorStack is used for stack-allocated results.

#include "Projectile.h"

#include "Pawn.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(Projectile);

using namespace Unigine;
using namespace Math;

// Position is updated along forward direction, lifetime is decremented (thread-safe).
void Projectile::updateSyncThread()
{
	// UpdateSyncThread runs in the update thread, safe for transform modifications.
	// Use this for physics-style updates that modify node positions.
	float ifps = Game::getIFps();

	// Extract forward direction from transform matrix (Y axis = column 1)
	Vec3 direction = node->getWorldTransform().getColumn3(1);

	node->setWorldPosition(node->getWorldPosition() + direction * speed * ifps);

	lifetime = lifetime - ifps;
}

// Lifetime is checked, collision with Pawn is detected, damage is applied on hit.
void Projectile::update()
{
	// Lifetime process
	if (lifetime < 0)
	{
		// Destroy current node with its properties and components
		node.deleteLater();
		return;
	}

	// Bounding box intersection query for collision detection.
	// VectorStack is stack-allocated (faster than heap Vector, but fixed capacity).
	VectorStack<NodePtr> nodes;
	World::getIntersection(node->getWorldBoundBox(), nodes);
	if (nodes.size() > 1)	// Self is always in the list, so check for > 1
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			Pawn *pawn = getComponent<Pawn>(nodes[i]);
			if (pawn)
			{
				// Hit the player!
				pawn->hit(damage);

				// ...and destroy current node
				node.deleteLater();
				return;
			}
		}
	}
}

// Material is applied to all surfaces of the projectile mesh.
void Projectile::setMaterial(const MaterialPtr &mat)
{
	checked_ptr_cast<Object>(node)->setMaterial(mat, "*");
}
