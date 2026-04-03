// Three equivalent approaches to node movement: using translate/rotate,
// setPosition/setRotation, and setTransform. All achieve the same result
// but demonstrate different API patterns for controlling node transforms.

#include "SimpleMovement.h"

#include "UnigineConsole.h"
#include "UnigineGame.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SimpleMovement);
REGISTER_COMPONENT(SimpleMovement2);
REGISTER_COMPONENT(SimpleMovement3);

/*=============== Simple Movement variant 1 ===============*/

// WASD input is processed; node is moved using translate/rotate methods.
void SimpleMovement::update()
{
	// Skip input when console is active
	if (Console::isActive())
		return;

	// Subtraction gives -1, 0, or +1: W alone = 1, S alone = -1, both or neither = 0
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		// Y axis is forward in local space; sign determines direction
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	// Same pattern for rotation: A = +1 (left), D = -1 (right), both/neither = 0
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		// Z axis rotation (yaw); sign determines turn direction
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

// translate() moves node in its local coordinate space.
void SimpleMovement::move(const Unigine::Math::Vec3& dir)
{
	Vec3 delta_movement = dir * velocity * Game::getIFps();
	// Simplest approach: translate handles local-to-world transformation
	node->translate(delta_movement);
}

// rotate() applies incremental rotation to current orientation.
void SimpleMovement::turn(const Unigine::Math::vec3& dir)
{
	vec3 delta_rotation = dir * angular_velocity * Game::getIFps();
	node->rotate(delta_rotation);
}


/*=============== Simple Movement variant 2 ===============*/

// WASD input is processed; node is moved using setPosition/setRotation.
void SimpleMovement2::update()
{
	if (Console::isActive())
		return;

	// Subtraction gives -1, 0, or +1: W alone = 1, S alone = -1, both or neither = 0
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	// Same pattern for rotation: A = +1 (left), D = -1 (right), both/neither = 0
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

// setPosition() requires manual local-to-world transformation of direction.
void SimpleMovement2::move(const Unigine::Math::Vec3& dir)
{
	// Must rotate direction by node's orientation to move in local space
	Vec3 deltaMovement = node->getRotation() * (dir * velocity * Game::getIFps());
	Vec3 oldPosition = node->getPosition();
	node->setPosition(oldPosition + deltaMovement);
}

// setRotation() multiplies quaternions to combine rotations.
void SimpleMovement2::turn(const Unigine::Math::vec3& dir)
{
	float deltaRotation = angular_velocity * Game::getIFps();
	quat oldRotation = node->getRotation();
	// Quaternion multiplication applies delta rotation to current orientation
	node->setRotation(oldRotation * quat(dir, deltaRotation));
}


/*=============== Simple Movement variant 3 ===============*/

// WASD input is processed; node is moved using setTransform with matrices.
void SimpleMovement3::update()
{
	if (Console::isActive())
		return;

	// Subtraction gives -1, 0, or +1: W alone = 1, S alone = -1, both or neither = 0
	int move_sign = int(Input::isKeyPressed(Input::KEY_W)) - int(Input::isKeyPressed(Input::KEY_S));
	if (move_sign)
	{
		move(Vec3(0.0f, float(move_sign), 0.0f));
	}

	// Same pattern for rotation: A = +1 (left), D = -1 (right), both/neither = 0
	int rotate_sign = int(Input::isKeyPressed(Input::KEY_A)) - int(Input::isKeyPressed(Input::KEY_D));
	if (rotate_sign)
	{
		turn(vec3(0.0f, 0.0f, float(rotate_sign)));
	}
}

// setTransform() applies translation matrix to current transform.
void SimpleMovement3::move(const Unigine::Math::Vec3& dir)
{
	Mat4 transform = node->getTransform();
	Vec3 delta_movement = dir * velocity * Game::getIFps();
	// Create translation matrix from delta position
	Mat4 delta_transform = translate(delta_movement);
	// Matrix multiplication applies local-space movement
	node->setTransform(transform * delta_transform);
}

// setTransform() applies rotation matrix to current transform.
void SimpleMovement3::turn(const Unigine::Math::vec3& dir)
{
	Mat4 transform = node->getTransform();
	float deltaRotation = angular_velocity * Game::getIFps();
	// Create rotation matrix from axis and angle
	Mat4 delta_transform = Unigine::Math::rotate(Vec3(dir), deltaRotation);
	node->setTransform(transform * delta_transform);
}
