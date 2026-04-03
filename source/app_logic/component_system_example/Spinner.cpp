// Rotating turret that spawns projectiles at random intervals. Demonstrates three
// methods to modify component parameters: direct C++ access, node property lookup,
// and component property reference. Difficulty increases over time via spawn rate.

#include "Spinner.h"

#include "Projectile.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(Spinner);

using namespace Unigine;
using namespace Math;

// Material is cached and random spawn/color parameters are initialized.
void Spinner::init()
{
	// Get current material (from the first surface)
	ObjectPtr obj = checked_ptr_cast<Object>(node);
	if (obj && obj->getNumSurfaces() > 0)
		material = obj->getMaterialInherit(0);

	// Init randoms
	time_to_spawn = Game::getRandomFloat(min_spawn_delay, max_spawn_delay);
	color_offset = Game::getRandomFloat(0, 360.0f);
	start_turn_speed = turn_speed;
}

// Rotation is applied, color is cycled, and projectiles are spawned at intervals.
void Spinner::update()
{
	// Rotate spinner
	float ifps = Game::getIFps();
	turn_speed = turn_speed + acceleration * ifps;
	node->setRotation(node->getRotation() * quat(0, 0, turn_speed * ifps));

	// Change color
	int id = material->findParameter("albedo_color");
	if (id != -1)
	{
		float hue = Math::mod(Game::getTime() * 60.0f + color_offset, 360.0f);
		material->setParameterFloat4(id, vec4(hsv2rgb(hue, 1, 1), 1.0f));
	}

	// Spawn projectiles
	time_to_spawn -= ifps;
	if (time_to_spawn < 0 && spawn_node.get())
	{
		// Reset timer and increase difficulty
		time_to_spawn = Game::getRandomFloat(min_spawn_delay, max_spawn_delay)
			/ (turn_speed / start_turn_speed);

		// Create node
		NodePtr spawned = spawn_node.get()->clone();
		spawned->setEnabled(1);
		spawned->setWorldTransform(node->getWorldTransform());

		// Create component
		Projectile *proj_component = addComponent<Projectile>(spawned);

		// Three patterns for modifying component parameters:
		// 1) Direct C++ access (fastest, type-safe)
		proj_component->speed = Game::getRandomFloat(proj_component->speed * 0.5f,
			proj_component->speed * 1.5f);

		// 2) Via node's property list (slower, for dynamic lookup by name)
		for (int i = 0; i < spawned->getNumProperties(); i++)
		{
			PropertyPtr prop = spawned->getProperty(i);
			if (prop && (!strcmp(prop->getName(), "Projectile") || prop->isParent("Projectile")))
				prop->getParameterPtr("damage")->setValueInt(Game::getRandomInt(75, 100));
		}

		// 3) Via component's property reference (cleaner than #2)
		PropertyPtr proj_property = proj_component->getProperty();
		proj_property->getParameterPtr("lifetime")
			->setValueFloat(Game::getRandomFloat(5.0f, 10.0f));

		// Call public method of another component
		proj_component->setMaterial(material);
	}
}

// HSV color is converted to RGB. H: [0, 360), S,V: [0, 1].
vec3 Spinner::hsv2rgb(float h, float s, float v)
{
	float p, q, t, fract;

	h /= 60.0f;
	fract = h - Math::floor(h);

	p = v * (1.0f - s);
	q = v * (1.0f - s * fract);
	t = v * (1.0f - s * (1.0f - fract));

	if (0.0f <= h && h < 1.0f) return vec3(v, t, p);
	else if (1.0f <= h && h < 2.0f) return vec3(q, v, p);
	else if (2.0f <= h && h < 3.0f) return vec3(p, v, t);
	else if (3.0f <= h && h < 4.0f) return vec3(p, q, v);
	else if (4.0f <= h && h < 5.0f) return vec3(t, p, v);
	else if (5.0f <= h && h < 6.0f) return vec3(v, p, q);
	else return vec3(0, 0, 0);
}
