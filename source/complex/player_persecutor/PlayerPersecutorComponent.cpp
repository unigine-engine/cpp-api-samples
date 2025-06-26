#include "PlayerPersecutorComponent.h"
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(PlayerPersecutorComponent);

void PlayerPersecutorComponent::init()
{
	camera = checked_ptr_cast<PlayerDummy>(node);
	
	shape = Unigine::ShapeSphere::create();
	shape->setContinuous(0);
	shape->setCenter(node->getWorldPosition());

	direction = vec3_right;
	distance = 4.0f;
	phi_direction = vec3_right;
	theta_angle = 0.0f;

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

void PlayerPersecutorComponent::update()
{
	if (!target_node || !camera)
		return;
	
	target = target_node->getWorldTransform() * Vec3(anchor);

	BodyRigidPtr body = target_node->getObjectBodyRigid();
	if (body)
		camera->setVelocity(body->getLinearVelocity());
	else
		camera->setVelocity(vec3_zero);

	float phi = 0.0f;
	float theta = 0.0f;

	if (controlled && camera->isMainPlayer() && Input::isMouseCursorHide())
	{
		// direction
		phi = Input::getMouseDeltaPosition().x;
		theta = Input::getMouseDeltaPosition().y;
	}

	float old_min_theta_angle = getMinThetaAngle();
	float old_max_theta_angle = getMaxThetaAngle();

	// fixed angles
	if (isFixed())
	{
		theta_angle += theta;
		theta_angle = clamp(theta_angle, min_theta_angle, max_theta_angle);
		theta += theta_angle - getThetaAngle();

		setMinThetaAngle(theta_angle);
		setMaxThetaAngle(theta_angle);
	}

	// adaptive collision-aware rotation adjustment
	do
	{
		// adaptive step
		float max_distance = max(getCollisionRadius(), getDistance());
		float min_distance = min(getCollisionRadius(), getDistance());

		float angle = max(Math::atan(max_distance / min_distance) * Consts::RAD2DEG * 0.5f, Consts::EPS);
		float p = clamp(phi, -angle, angle);
		float t = clamp(theta, -angle, angle);
		phi -= p;
		theta -= t;

		Vector<ShapeContactPtr> contacts;
		shape->getCollision(contacts, Game::getIFps());

		// update constraints
		update_distance();
		update_angles(p, t);

		// world collision
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

		// shape position
		shape->setCenter(position);
	} while (Math::abs(phi) > Consts::EPS || Math::abs(theta) > Consts::EPS);

	setMinThetaAngle(old_min_theta_angle);
	setMaxThetaAngle(old_max_theta_angle);

	// applying transform
	flushTransform();
}

void PlayerPersecutorComponent::shutdown()
{
	shape.deleteLater();
}

//////////////////////////////////////////////////////////////////////////
// Parameters
//////////////////////////////////////////////////////////////////////////

void PlayerPersecutorComponent::setFixed(int f)
{
	fixed = f;
	update_transform();
}

void PlayerPersecutorComponent::setTarget(NodePtr n)
{
	target_node = n;
	update_transform();
}

void PlayerPersecutorComponent::setAnchor(const vec3& a)
{
	anchor = a;
	update_transform();
}

//////////////////////////////////////////////////////////////////////////
// Dynamic
//////////////////////////////////////////////////////////////////////////

void PlayerPersecutorComponent::setDistance(float d)
{
	distance = clamp(d, min_distance, max_distance);
	position = target - Vec3(direction * distance);

	flushTransform();
}

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

float PlayerPersecutorComponent::getPhiAngle() const
{
	if (!target_node)
		return 0.0f;

	vec3 up = camera->getUp();

	if (isFixed())
	{
		vec3 d0 = rotation(target_node.get()->getWorldTransform()) * phi_direction;
		vec3 d1 = direction;
		d0 = normalize(d0 - up * dot(up, d0));
		d1 = normalize(d1 - up * dot(up, d1));

		return Math::acos(clamp(dot(d0, d1), -1.0f, 1.0f)) * Math::sign(dot(up, cross(d0, d1))) * Consts::RAD2DEG;
	}
	else
	{
		vec3 tangent, binormal;
		orthoBasis(up, tangent, binormal);

		return Math::atan2(dot(direction, tangent), dot(direction, binormal)) * Consts::RAD2DEG;
	}
}

void PlayerPersecutorComponent::setThetaAngle(float angle)
{
	theta_angle = clamp(angle, min_theta_angle, max_theta_angle);
	float theta = theta_angle - getThetaAngle();

	direction = quat(cross(camera->getUp(), direction), theta) * direction;
	position = target - Vec3(direction * distance);

	flushTransform();
}

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
// Update
//////////////////////////////////////////////////////////////////////////

void PlayerPersecutorComponent::update_distance()
{
	// ortho basis
	vec3 tangent, binormal;
	orthoBasis(camera->getUp(), tangent, binormal);

	// update direction
	direction = vec3(target - position);
	distance = length(direction);
	if (length(vec2(direction)) > Consts::EPS)
		direction /= distance;
	else
		direction = binormal;

	// clamp distance
	distance = clamp(distance, min_distance, max_distance);

	// update position
	position = target - Vec3(direction * distance);
}

void PlayerPersecutorComponent::update_angles(float phi, float theta)
{
	vec3 up = camera->getUp();

	// theta angle
	float theta_angle = getThetaAngle() + theta;

	// clamp theta angle
	if (theta_angle < min_theta_angle)
		theta += min_theta_angle - theta_angle;
	else if (theta_angle > max_theta_angle)
		theta += max_theta_angle - theta_angle;

	// fixed angles
	if (isFixed())
	{
		quat transform = quat(target_node->getWorldTransform());
		phi_direction = normalize(inverse(transform) * quat(up, -phi) * transform) * phi_direction;
		phi = getPhiAngle();
	}

	// update transformation
	direction = (quat(up, -phi) * quat(cross(up, direction), theta)) * direction;
	position = target - Vec3(direction * distance);
}

//////////////////////////////////////////////////////////////////////////
// Transformation
//////////////////////////////////////////////////////////////////////////

void PlayerPersecutorComponent::update_transform()
{
	// update transformation
	if (flush == 0 && target_node)
	{
		// target position
		target = target_node->getWorldTransform() * Vec3(anchor);

		// decompose transformation
		position = node->getWorldTransform().getColumn3(3); 

		// update distance
		update_distance();

		// phi direction
		phi_direction = rotation(target_node->getIWorldTransform()) * direction;

		// update angles
		update_angles(0.0f, 0.0f);

		// theta angle
		theta_angle = getThetaAngle();
	}
}

void PlayerPersecutorComponent::flushTransform()
{
	flush = 1;
	node->setWorldTransform(setTo(position, position + Vec3(direction), camera->getUp()));
	flush = 0;
}
