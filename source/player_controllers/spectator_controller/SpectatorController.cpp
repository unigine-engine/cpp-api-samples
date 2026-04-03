// Free-flying spectator camera with 6DOF movement and optional sphere collision.
// Uses fixed timestep simulation for frame-rate independent movement. Supports
// acceleration, damping, and sprint mode. Handles external transform changes
// by syncing internal state with node transform.

#include "SpectatorController.h"
#include <UnigineGame.h>
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SpectatorController)

// Creates collision sphere, converts string key names to input enums,
// gets player reference, and initializes internal state from node transform.
void SpectatorController::init()
{
	// Create physics shape
	p_shape_sphere = ShapeSphere::create(collision_radius.get());
	p_shape_sphere->setContinuous(0);

	// Assign input keys from string parameters
	_forward_key = Input::getKeyByName(forward_key.get());
	_backward_key = Input::getKeyByName(backward_key.get());
	_right_key = Input::getKeyByName(right_key.get());
	_left_key = Input::getKeyByName(left_key.get());
	_up_key = Input::getKeyByName(up_key.get());
	_down_key = Input::getKeyByName(down_key.get());
	_turn_up_key = Input::getKeyByName(turn_up_key.get());
	_turn_down_key = Input::getKeyByName(turn_down_key.get());
	_turn_left_key = Input::getKeyByName(turn_left_key.get());
	_turn_right_key = Input::getKeyByName(turn_right_key.get());
	_accelerate_key = Input::getKeyByName(accelerate_key.get());

	p_player = checked_ptr_cast<Player>(node);

	on_transform_changed();	// Get starting parameters of node
}

// Per-frame update: detects external transform changes, processes input,
// applies movement, and syncs transform back to node.
void SpectatorController::update()
{
	// If somebody changed our position outside, update all internal params
	if (transform != node->getTransform())
		on_transform_changed();

	updateControls();
	flushTransform();

	transform = node->getTransform();
}

// Cleans up collision shape.
void SpectatorController::shutdown()
{
	p_shape_sphere.deleteLater();	// Delete the shape we created
}

// Handles user input and triggers movement updates. Reads mouse delta for rotation,
// keyboard for movement impulse, then runs fixed-timestep physics simulation.
void SpectatorController::updateControls()
{
	vec3 up = p_player->getUp();

	vec3 impulse = vec3_zero;

	vec3 tangent, binormal;
	orthoBasis(p_player->getUp(), tangent, binormal);

	if (is_controlled.get() && !Console::isActive())
	{
		if (Input::isMouseCursorHide())
		{ 
			phi_angle += Input::getMouseDeltaPosition().x * mouse_sensitivity.get();
			theta_angle += Input::getMouseDeltaPosition().y * mouse_sensitivity.get();
		}

		theta_angle += turning * Game::getIFps() * (Input::isKeyPressed(_turn_down_key) - Input::isKeyPressed(_turn_up_key));
		theta_angle = clamp(theta_angle, min_theta_angle, max_theta_angle);

		phi_angle += turning * Game::getIFps() * (Input::isKeyPressed(_turn_right_key) - Input::isKeyPressed(_turn_left_key));

		vec3 x = (quat(up, -phi_angle) * quat(tangent, -theta_angle)) * binormal;
		vec3 y = normalizeValid(cross(up, x));
		vec3 z = normalizeValid(cross(x, y));

		direction = x;

		impulse += x * float(Input::isKeyPressed(_forward_key) - Input::isKeyPressed(_backward_key));
		impulse += y * float(Input::isKeyPressed(_left_key) - Input::isKeyPressed(_right_key));
		impulse += z * float(Input::isKeyPressed(_up_key) - Input::isKeyPressed(_down_key));

		impulse.normalizeValid();

		impulse *= Input::isKeyPressed(_accelerate_key) ? max_velocity.get() : min_velocity.get();
	}

	float time = Game::getIFps();

	float target_velocity = length(impulse);

	Vec3 player_velocity = Vec3(p_player->getVelocity());

	// Use do-while to ensure at least one update is processed,
	// even when the remaining time is very small (e.g., at high FPS).
	do
	{
		// Clamp the simulation step to a maximum fixed time interval (PLAYER_SPECTATOR_IFPS).
		// This prevents instability or large jumps in movement and collisions when frame time is high (e.g., during frame drops).
		float ifps = min(time, PLAYER_SPECTATOR_IFPS);
		time -= ifps;
		update_movement(impulse, ifps, target_velocity);
	} while (time > Consts::EPS);
}


// Applies position and direction to node transform if changed since last frame.
void SpectatorController::flushTransform()
{
	vec3 up = p_player->getUp();

	if (last_position != position || last_direction != direction || last_up != up)
	{
		node->setWorldTransform(setTo(position, position + Vec3(direction), up));
		on_transform_changed(); // update all internal params

		last_position = position;
		last_direction = direction;
		last_up = up;
	}
}

// Syncs internal state (position, direction, angles) from node's world transform.
// Called on init and whenever external code modifies the transform.
void SpectatorController::on_transform_changed()
{
	vec3 up = p_player->getUp();

	vec3 tangent, binormal;
	orthoBasis(up, tangent, binormal);

	position = node->getWorldTransform().getColumn3(3);
	direction = normalizeValid(vec3(-node->getWorldTransform().getColumn3(2)));

	phi_angle = Math::atan2(dot(direction, tangent), dot(direction, binormal)) * Consts::RAD2DEG;
	theta_angle = Math::acos(clamp(dot(direction, up), -1.0f, 1.0f)) * Consts::RAD2DEG - 90.0f;
	theta_angle = clamp(theta_angle, min_theta_angle, max_theta_angle);
	direction = (quat(up, -phi_angle) * quat(tangent, -theta_angle)) * binormal;

	last_position = position;
	last_direction = direction;
	last_up = up;
}

// Applies velocity with acceleration/damping and resolves sphere collisions.
// Called in fixed timestep loop to ensure consistent physics behavior.
void SpectatorController::update_movement(const vec3& impulse, float ifps, float target_velocity)
{
	float old_velocity = length(p_player->getVelocity());

	p_player->setVelocity(p_player->getVelocity() + impulse * acceleration.get() * ifps);

	float current_velocity = length(p_player->getVelocity());
	if (target_velocity < Consts::EPS || current_velocity > target_velocity)
		p_player->setVelocity(p_player->getVelocity() * Math::exp(-damping * ifps));

	current_velocity = length(p_player->getVelocity());
	if (current_velocity > old_velocity && current_velocity > target_velocity)
		p_player->setVelocity(p_player->getVelocity() * target_velocity / current_velocity);

	if (current_velocity < Consts::EPS)
		p_player->setVelocity(vec3_zero);

	position += Vec3(p_player->getVelocity() * ifps);

	contacts.clear();

	if (p_shape_sphere->isEnabled() && is_collided.get())
	{
		for (int i = 0; i < PLAYER_SPECTATOR_COLLISIONS; i++)
		{
			p_shape_sphere->setCenter(position);
			p_shape_sphere->getCollision(contacts, ifps);
			if (contacts.size() == 0)
				break;

			// Calculate the inverse of the number of contacts to evenly distribute the total push-out
			// This prevents applying the full depth for every contact, which would overcompensate the position
			float inum_contacts = 1.0f / Math::itof(contacts.size());
			for (int j = 0; j < contacts.size(); j++)
			{
				const ShapeContactPtr c = contacts[j];

				// Push the player out along the contact normal, scaled by penetration depth and evenly divided by contact count
				position += Vec3(c->getNormal() * (c->getDepth() * inum_contacts));

				// Remove the velocity component that's directed into the contact surface
				// This prevents the player from continuing to move into the object
				p_player->setVelocity(p_player->getVelocity() - c->getNormal() * dot(p_player->getVelocity(), c->getNormal()));
			}
		}
	}

	p_shape_sphere->setCenter(position);
}

// Sets horizontal rotation angle (yaw) and updates view direction.
void SpectatorController::setPhiAngle(float newAngle)
{
	newAngle = newAngle - phi_angle;
	direction = quat(p_player->getUp(), newAngle) * direction;
	phi_angle += newAngle;

	flushTransform();
}

// Sets vertical rotation angle (pitch), clamped to min/max theta limits.
void SpectatorController::setThetaAngle(float newAngle)
{
	newAngle = clamp(newAngle, min_theta_angle.get(), max_theta_angle.get()) - theta_angle;
	direction = quat(cross(p_player->getUp(), direction), newAngle) * direction;
	theta_angle += newAngle;

	flushTransform();
}

// Sets view direction from a vector, decomposing into phi/theta angles.
void SpectatorController::setViewDirection(const vec3 &newView)
{
	direction = normalizeValid(newView);

	// Ortho basis
	vec3 tangent, binormal;
	orthoBasis(p_player->getUp(), tangent, binormal);

	// Decompose view direction into spherical angles
	phi_angle = Math::atan2(dot(direction, tangent), dot(direction, binormal)) * Consts::RAD2DEG;
	theta_angle = Math::acos(clamp(dot(direction, p_player->getUp()), -1.0f, 1.0f)) * Consts::RAD2DEG - 90.0f;
	theta_angle = clamp(theta_angle, min_theta_angle.get(), max_theta_angle.get());

	flushTransform();
}

//////////////////////////////////////////////////////////////////////////
// Collision contact accessors
//////////////////////////////////////////////////////////////////////////

// Returns full contact data at the specified index.
const ShapeContactPtr SpectatorController::getContact(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContact(): bad contact number");
	return contacts[num];
}

// Returns penetration depth at the specified contact.
float SpectatorController::getContactDepth(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactDepth(): bad contact number");
	return contacts[num]->getDepth();
}

// Returns surface normal at the specified contact.
vec3 SpectatorController::getContactNormal(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactNormal(): bad contact number");
	return contacts[num]->getNormal();
}

// Returns the object that was collided with at the specified contact.
ObjectPtr SpectatorController::getContactObject(int num)
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactObject(): bad contact number");
	return contacts[num]->getObject();
}

// Returns the world-space position of the specified contact.
Vec3 SpectatorController::getContactPoint(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactPoint(): bad contact number");
	return contacts[num]->getPoint();
}

// Returns the collision shape involved in the specified contact.
ShapePtr SpectatorController::getContactShape(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactShape(): bad contact number");
	return contacts[num]->getShape0();
}

// Returns the surface index of the collided object at the specified contact.
int SpectatorController::getContactSurface(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerSpectator::getContactSurface(): bad contact number");
	return contacts[num]->getSurface();
}



