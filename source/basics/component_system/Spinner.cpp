#include "Spinner.h"

#include "Projectile.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(Spinner);

using namespace Unigine;
using namespace Math;

void Spinner::init()
{
	// get current material (from the first surface)
	ObjectPtr obj = checked_ptr_cast<Object>(node);
	if (obj && obj->getNumSurfaces() > 0)
		material = obj->getMaterialInherit(0);

	// init randoms
	time_to_spawn = Game::getRandomFloat(min_spawn_delay, max_spawn_delay);
	color_offset = Game::getRandomFloat(0, 360.0f);
	start_turn_speed = turn_speed;
}

void Spinner::update()
{
	// rotate spinner
	float ifps = Game::getIFps();
	turn_speed = turn_speed + acceleration * ifps;
	node->setRotation(node->getRotation() * quat(0, 0, turn_speed * ifps));

	// change color
	int id = material->findParameter("albedo_color");
	if (id != -1)
	{
		float hue = Math::mod(Game::getTime() * 60.0f + color_offset, 360.0f);
		material->setParameterFloat4(id, vec4(hsv2rgb(hue, 1, 1), 1.0f));
	}

	// spawn projectiles
	time_to_spawn -= ifps;
	if (time_to_spawn < 0 && spawn_node.get())
	{
		// reset timer and increase difficulty
		time_to_spawn = Game::getRandomFloat(min_spawn_delay, max_spawn_delay)
			/ (turn_speed / start_turn_speed);

		// create node
		NodePtr spawned = spawn_node.get()->clone();
		spawned->setEnabled(1);
		spawned->setWorldTransform(node->getWorldTransform());

		// create component
		Projectile *proj_component = addComponent<Projectile>(spawned);

		// there are three ways to change variables inside another component:
		// 1) direct change via component (fast, easy)
		proj_component->speed = Game::getRandomFloat(proj_component->speed * 0.5f,
			proj_component->speed * 1.5f);

		// 2) change via property of the node (more slow, more awkward)
		for (int i = 0; i < spawned->getNumProperties(); i++)
		{
			PropertyPtr prop = spawned->getProperty(i);
			if (prop && (!strcmp(prop->getName(), "Projectile") || prop->isParent("Projectile")))
				prop->getParameterPtr("damage")->setValueInt(Game::getRandomInt(75, 100));
		}

		// 3) change via property of the component
		PropertyPtr proj_property = proj_component->getProperty();
		proj_property->getParameterPtr("lifetime")
			->setValueFloat(Game::getRandomFloat(5.0f, 10.0f));

		// call public method of another component
		proj_component->setMaterial(material);
	}
}

// H - [0, 360), S,V - [0, 1]
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
