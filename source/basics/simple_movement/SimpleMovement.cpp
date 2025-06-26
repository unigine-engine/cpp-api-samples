#include "SimpleMovement.h"

#include "UnigineConsole.h"
#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SimpleMovement);
REGISTER_COMPONENT(SimpleMovement2);
REGISTER_COMPONENT(SimpleMovement3);

/*=============== Simple Movement variant 1 ===============*/

/*
* Move with Node::translate method
* Turn with Node::rotate method
*/

void SimpleMovement::update()
{
	if (Console::isActive())
		return;

	/* determine forward or backward move */
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	/* determine left or right turn */
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

void SimpleMovement::move(const Unigine::Math::Vec3& dir)
{
	Vec3 delta_movement = dir * velocity * Game::getIFps();

	node->translate(delta_movement);
}

void SimpleMovement::turn(const Unigine::Math::vec3& dir)
{
	vec3 delta_rotation = dir * angular_velocity * Game::getIFps();
	node->rotate(delta_rotation);
}


/*=============== Simple Movement variant 2 ===============*/

/*
* Move with Node::setPosition method
* Turn with Node::setRotation method
*/

void SimpleMovement2::update()
{
	if (Console::isActive())
		return;

	/* determine forward or backward move */
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	/* determine left or right turn */
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

void SimpleMovement2::move(const Unigine::Math::Vec3& dir)
{
	Vec3 deltaMovement = node->getRotation() * (dir * velocity * Game::getIFps());
	Vec3 oldPosition = node->getPosition();
	node->setPosition(oldPosition + deltaMovement);
}

void SimpleMovement2::turn(const Unigine::Math::vec3& dir)
{
	float deltaRotation = angular_velocity * Game::getIFps();
	quat oldRotation = node->getRotation();
	node->setRotation(oldRotation * quat(dir, deltaRotation));
}


/*=============== Simple Movement variant 3 ===============*/

/*
* Move with Node::setTransform method
* Turn with Node::setTransform method
*/

void SimpleMovement3::update()
{
	if (Console::isActive())
		return;

	/* determine forward or backward move */
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	/* determine left or right turn */
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

void SimpleMovement3::move(const Unigine::Math::Vec3& dir)
{
	Mat4 transform = node->getTransform();
	Vec3 delta_movement = dir * velocity * Game::getIFps();
	Mat4 delta_transform = translate(delta_movement);
	node->setTransform(transform * delta_transform);
}

void SimpleMovement3::turn(const Unigine::Math::vec3& dir)
{
	Mat4 transform = node->getTransform();
	float deltaRotation = angular_velocity * Game::getIFps();
	Mat4 delta_transform = Unigine::Math::rotate(Vec3(dir), deltaRotation);
	node->setTransform(transform * delta_transform);
}
