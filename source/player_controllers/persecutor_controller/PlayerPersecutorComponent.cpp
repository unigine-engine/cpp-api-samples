// Third-person camera that follows a target node using spherical coordinates.
// Supports fixed (locked to target rotation) and free orbiting modes, with
// collision detection to prevent camera from clipping through geometry.
// Uses sphere shape for collision queries and iterative position correction.

#include "PlayerPersecutorComponent.h"
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(PlayerPersecutorComponent);

// Creates collision sphere and initializes spherical coordinate system.
// Must be attached to a PlayerDummy node.
void PlayerPersecutorComponent::init()
{
	camera = checked_ptr_cast<PlayerDummy>(node);

	// Collision sphere for detecting obstacles between camera and target
	shape = Unigine::ShapeSphere::create();
	shape->setContinuous(0);
	shape->setCenter(node->getWorldPosition());

	// Initialize spherical coordinate state
	direction = vec3_right;
	distance = 4.0f;
	phi_direction = vec3_right;
	theta_angle = 0.0f;

	// Apply editor parameters
	setFixed(fixed);
	setCollision(collision);
	setCollisionMask(collision_mask);
	setCollisionRadius(0.5f);

	setAnchor(anchor);
	setMinDistance(min_distance);
	setMaxDistance(max_distance);
	setMinThetaAngle(min_theta_angle);
	setMaxThetaAngle(max_theta_angle);
	setTurning(90.0f);
}

// Per-frame update: reads mouse input, applies rotation in adaptive steps,
// resolves collisions iteratively, and updates camera transform.
void PlayerPersecutorComponent::update()
{
	if (!target_node || !camera)
		return;

	// Calculate target anchor point in world space
	target = target_node->getWorldTransform() * Vec3(anchor);

	// Match camera velocity to target for smooth motion
	BodyRigidPtr body = target_node->getObjectBodyRigid();
	if (body)
		camera->setVelocity(body->getLinearVelocity());
	else
		camera->setVelocity(vec3_zero);

	// Read rotation input from mouse
	float phi = 0.0f;
	float theta = 0.0f;

	if (controlled && camera->isMainPlayer() && Input::isMouseCursorHide())
	{
		phi = Input::getMouseDeltaPosition().x;
		theta = Input::getMouseDeltaPosition().y;
	}

	float old_min_theta_angle = getMinThetaAngle();
	float old_max_theta_angle = getMaxThetaAngle();

	// Fixed angles: lock theta to target rotation
	if (isFixed())
	{
		theta_angle += theta;
		theta_angle = clamp(theta_angle, min_theta_angle, max_theta_angle);
		theta += theta_angle - getThetaAngle();

		setMinThetaAngle(theta_angle);
		setMaxThetaAngle(theta_angle);
	}

	// Adaptive collision-aware rotation adjustment
	do
	{
		// Adaptive step size based on collision radius and distance
		float max_distance = max(getCollisionRadius(), getDistance());
		float min_distance = min(getCollisionRadius(), getDistance());

		float angle = max(Math::atan(max_distance / min_distance) * Consts::RAD2DEG * 0.5f, Consts::EPS);
		float p = clamp(phi, -angle, angle);
		float t = clamp(theta, -angle, angle);
		phi -= p;
		theta -= t;

		Vector<ShapeContactPtr> contacts;
		shape->getCollision(contacts, Game::getIFps());

		// Update constraints
		update_distance();
		update_angles(p, t);

		// World collision: iteratively resolve penetrations
		contacts.clear();
		if (getCollision())
		{
			for (int i = 0; i < PLAYER_PERSECUTOR_COLLISIONS; i++)
			{
				shape->setCenter(position);
				shape->getCollision(contacts, 0.0f);
				if (contacts.size() == 0)
					break;
				float inum_contacts = 1.0f / Math::itof(contacts.size());
				for (int j = 0; j < contacts.size(); j++)
				{
					const ShapeContact& c = *contacts[j].get();
					position += Vec3(c.getNormal() * (c.getDepth() * inum_contacts));
				}
				update_distance();
				if (isFixed())
					update_angles(-getPhiAngle(), 0.0f);
				else
					update_angles(0.0f, 0.0f);
			}
		}

		// Shape position: sync collision shape with camera
		shape->setCenter(position);
	} while (Math::abs(phi) > Consts::EPS || Math::abs(theta) > Consts::EPS);

	setMinThetaAngle(old_min_theta_angle);
	setMaxThetaAngle(old_max_theta_angle);

	// Applying transform
	flushTransform();
}

// Cleans up collision shape.
void PlayerPersecutorComponent::shutdown()
{
	shape.deleteLater();
}

//////////////////////////////////////////////////////////////////////////
// Parameters - setters that update internal transform state
//////////////////////////////////////////////////////////////////////////

// Switches between fixed (locked to target) and free orbiting modes.
void PlayerPersecutorComponent::setFixed(int f)
{
	fixed = f;
	update_transform();
}

// Sets the target node to follow.
void PlayerPersecutorComponent::setTarget(NodePtr n)
{
	target_node = n;
	update_transform();
}

// Sets the anchor point offset on the target node.
void PlayerPersecutorComponent::setAnchor(const vec3& a)
{
	anchor = a;
	update_transform();
}

//////////////////////////////////////////////////////////////////////////
// Dynamic - runtime spherical coordinate manipulation
//////////////////////////////////////////////////////////////////////////

// Sets camera distance from target (clamped to min/max).
void PlayerPersecutorComponent::setDistance(float d)
{
	distance = clamp(d, min_distance, max_distance);
	position = target - Vec3(direction * distance);

	flushTransform();
}

// Sets horizontal orbit angle (phi) around target.
void PlayerPersecutorComponent::setPhiAngle(float angle)
{
	if (!target_node)
		return;

	float phi = angle - getPhiAngle();

	
	vec3 up = camera->getUp();

	quat transform = quat(target_node->getWorldTransform());
	phi_direction = (inverse(transform) * quat(up, phi) * transform) * phi_direction;

	direction = quat(up, -phi) * direction;
	position = target - Vec3(direction * distance);

	flushTransform();
}

// Returns horizontal orbit angle. In fixed mode, returns angle relative
// to target's orientation; in free mode, returns absolute world angle.
float PlayerPersecutorComponent::getPhiAngle() const
{
	if (!target_node)
		return 0.0f;

	vec3 up = camera->getUp();

	if (isFixed())
	{
		// Calculate angle between reference direction and current direction
		vec3 d0 = rotation(target_node.get()->getWorldTransform()) * phi_direction;
		vec3 d1 = direction;
		d0 = normalize(d0 - up * dot(up, d0));
		d1 = normalize(d1 - up * dot(up, d1));

		return Math::acos(clamp(dot(d0, d1), -1.0f, 1.0f)) * Math::sign(dot(up, cross(d0, d1))) * Consts::RAD2DEG;
	}
	else
	{
		// Calculate absolute angle in world space
		vec3 tangent, binormal;
		orthoBasis(up, tangent, binormal);

		return Math::atan2(dot(direction, tangent), dot(direction, binormal)) * Consts::RAD2DEG;
	}
}

// Sets vertical orbit angle (theta) - elevation above/below target.
void PlayerPersecutorComponent::setThetaAngle(float angle)
{
	theta_angle = clamp(angle, min_theta_angle, max_theta_angle);
	float theta = theta_angle - getThetaAngle();

	direction = quat(cross(camera->getUp(), direction), theta) * direction;
	position = target - Vec3(direction * distance);

	flushTransform();
}

// Returns vertical elevation angle (-90 = below target, +90 = above).
float PlayerPersecutorComponent::getThetaAngle() const
{
	return Math::acos(clamp(dot(direction, camera->getUp()), -1.0f, 1.0f)) * Consts::RAD2DEG - 90.0f;
}

void PlayerPersecutorComponent::setViewDirection(const vec3& view)
{
	direction = normalize(view);
	theta_angle = getThetaAngle();
	flushTransform();
}

void PlayerPersecutorComponent::setView(const vec3& view)
{
	direction = normalize(view);

	theta_angle = getThetaAngle();

	flushTransform();
}


//////////////////////////////////////////////////////////////////////////
// Contacts
//////////////////////////////////////////////////////////////////////////


const ShapeContactPtr& PlayerPersecutorComponent::getContact(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContact(): bad contact number");
	return contacts[num];
}

float PlayerPersecutorComponent::getContactDepth(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactDepth(): bad contact number");
	return contacts[num]->getDepth();
}

vec3 PlayerPersecutorComponent::getContactNormal(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactNormal(): bad contact number");
	return contacts[num]->getNormal();
}

ObjectPtr PlayerPersecutorComponent::getContactObject(int num)
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactObject(): bad contact number");
	return contacts[num]->getObject();
}

Vec3 PlayerPersecutorComponent::getContactPoint(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactPoint(): bad contact number");
	return contacts[num]->getPoint();
}

ShapePtr PlayerPersecutorComponent::getContactShape(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactShape(): bad contact number");
	return checked_ptr_cast<Shape>(contacts[num]->getShape1());
}

int PlayerPersecutorComponent::getContactSurface(int num) const
{
	assert(num >= 0 && num < getNumContacts() && "PlayerPersecutor::getContactSurface(): bad contact number");
	return contacts[num]->getSurface();
}

//////////////////////////////////////////////////////////////////////////
// Update - internal state recalculation methods
//////////////////////////////////////////////////////////////////////////

// Recalculates direction and distance from current positions, applies clamping.
void PlayerPersecutorComponent::update_distance()
{
	vec3 tangent, binormal;
	orthoBasis(camera->getUp(), tangent, binormal);

	// Calculate direction from position to target
	direction = vec3(target - position);
	distance = length(direction);
	if (length(vec2(direction)) > Consts::EPS)
		direction /= distance;
	else
		direction = binormal;

	// Apply distance constraints
	distance = clamp(distance, min_distance, max_distance);

	// Recalculate position from constrained distance
	position = target - Vec3(direction * distance);
}

// Applies phi (horizontal) and theta (vertical) rotation deltas with clamping.
void PlayerPersecutorComponent::update_angles(float phi, float theta)
{
	vec3 up = camera->getUp();

	// Theta angle: calculate new vertical angle
	float theta_angle = getThetaAngle() + theta;

	// Clamp theta angle to configured limits
	if (theta_angle < min_theta_angle)
		theta += min_theta_angle - theta_angle;
	else if (theta_angle > max_theta_angle)
		theta += max_theta_angle - theta_angle;

	// Fixed angles: update reference direction in target's local space
	if (isFixed())
	{
		quat transform = quat(target_node->getWorldTransform());
		phi_direction = normalize(inverse(transform) * quat(up, -phi) * transform) * phi_direction;
		phi = getPhiAngle();
	}

	// Update transformation: apply rotation to direction and recalculate position
	direction = (quat(up, -phi) * quat(cross(up, direction), theta)) * direction;
	position = target - Vec3(direction * distance);
}

//////////////////////////////////////////////////////////////////////////
// Transformation - sync between node and internal state
//////////////////////////////////////////////////////////////////////////

// Extracts spherical coordinates from current node transform.
// Called when target/anchor changes to reinitialize internal state.
void PlayerPersecutorComponent::update_transform()
{
	// Prevent recursive updates during flushTransform
	if (flush == 0 && target_node)
	{
		target = target_node->getWorldTransform() * Vec3(anchor);

		// Extract current position from node
		position = node->getWorldTransform().getColumn3(3);

		update_distance();

		// Store reference direction in target's local space (for fixed mode)
		phi_direction = rotation(target_node->getIWorldTransform()) * direction;

		update_angles(0.0f, 0.0f);
		theta_angle = getThetaAngle();
	}
}

// Applies internal state to node transform. Uses setTo() to look from
// position toward target while maintaining camera's up vector.
void PlayerPersecutorComponent::flushTransform()
{
	flush = 1;
	node->setWorldTransform(setTo(position, position + Vec3(direction), camera->getUp()));
	flush = 0;
}
