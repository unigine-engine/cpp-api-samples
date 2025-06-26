#include "Projectile.h"

#include "Pawn.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(Projectile);

using namespace Unigine;
using namespace Math;

void Projectile::updateSyncThread()
{
	// get delta time between frames
	float ifps = Game::getIFps();

	// get the direction vector of the mesh from the second column (y axis) of the transformation
	// matrix
	Vec3 direction = node->getWorldTransform().getColumn3(1);

	// move forward
	node->setWorldPosition(node->getWorldPosition() + direction * speed * ifps);

	// lifetime update
	lifetime = lifetime - ifps;
}

void Projectile::update()
{
	// lifetime process
	if (lifetime < 0)
	{
		// destroy current node with its properties and components
		node.deleteLater();
		return;
	}

	// check the intersection with nodes
	VectorStack<NodePtr> nodes; // VectorStack much more faster than Vector, but has some limits
	World::getIntersection(node->getWorldBoundBox(), nodes);
	if (nodes.size() > 1) // (because the current node is also in this list)
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			Pawn *pawn = getComponent<Pawn>(nodes[i]);
			if (pawn)
			{
				// hit the player!
				pawn->hit(damage);

				// ...and destroy current node
				node.deleteLater();
				return;
			}
		}
	}
}

void Projectile::setMaterial(const MaterialPtr &mat)
{
	checked_ptr_cast<Object>(node)->setMaterial(mat, "*");
}
