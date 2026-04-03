// Simulates an expanding spherical explosion that applies outward forces to
// nearby rigid bodies and triggers fracturing on destructible objects.
// The explosion expands over time, with force strength decreasing as the
// radius grows (inverse falloff from center).

#include "BodyFractureExplosion.h"
#include "../../utils/BodyFractureUnit.h"
#include "UnigineMathLibVec4.h"
#include <UniginePhysics.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(BodyFractureExplosion);

using namespace Unigine;

void BodyFractureExplosion::explode()
{
	isExploding = true;
}

void BodyFractureExplosion::init()
{
	radius = 0;
	isExploding = false;

	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);
}

void BodyFractureExplosion::updatePhysics()
{
	if (!isExploding)
		return;

	// Create a spherical query region at the current explosion radius
	Math::BoundSphere bound(Math::vec3(node->getWorldPosition()), radius);

	if (debug)
		Visualizer::renderBoundSphere(bound, Math::Mat4_identity, Math::vec4_red, 0.1f);

	// Force decreases linearly as explosion expands (stronger near center)
	auto actualPower = power * (1 - radius / max_radius);

	// Query all objects intersecting the explosion sphere
	Vector<ObjectPtr> objects;
	if (World::getIntersection(bound, objects))
	{
		for (auto object : objects)
		{
			auto body = object->getBody();
			if (!body)
				continue;

			// Calculate outward direction from explosion center to object
			auto dir = Math::vec3(object->getWorldPosition() - node->getWorldPosition());
			dir.normalizeValid();

			if (debug)
				Visualizer::renderDirection(object->getWorldPosition(), dir, Math::vec4_blue, 0.25f, true, 0.8f, false);

			// Trigger fracturing on destructible objects (impact direction is inward)
			if (auto fractureUnit = getComponent<BodyFractureUnit>(object))
			{
				fractureUnit->crack(actualPower, object->getWorldPosition(), -dir);
			}

			// Push rigid bodies outward from explosion center
			if (auto rigidBody = checked_ptr_cast<BodyRigid>(body))
			{
				rigidBody->addForce(dir * actualPower);
			}
		}
	}

	// Expand explosion radius over time, scaled by physics timestep
	radius = Math::min(radius + speed * Physics::getIFps(), max_radius);
	if (radius >= max_radius)
	{
		radius = 0;
		isExploding = false;
	}
}

void BodyFractureExplosion::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);
}
