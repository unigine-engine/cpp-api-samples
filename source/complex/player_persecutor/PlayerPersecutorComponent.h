#pragma once

#include <UnigineComponentSystem.h>

class PlayerPersecutorComponent : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PlayerPersecutorComponent, ComponentBase);
	
	PROP_PARAM(Toggle, controlled, 1, "", "Controlled player persecutor");
	
	PROP_GROUP("Target")
	PROP_PARAM(Node, target_node);
	PROP_PARAM(Vec3, anchor, Unigine::Math::vec3_zero);

	PROP_GROUP("Behavior")
	PROP_PARAM(Toggle, fixed, 0, "", "fixed angle between player persecutor and target");
	PROP_PARAM(Toggle, collision, 1, "", "Collision detection for player persecutor");
	PROP_PARAM(Mask, collision_mask, "collision", 1, "", "Collision Mask");
	PROP_PARAM(Float, min_distance, 0.0f);
	PROP_PARAM(Float, max_distance, 50.0f);
	PROP_PARAM(Float, min_theta_angle, -89.9f);
	PROP_PARAM(Float, max_theta_angle, 89.9f);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	void setFixed(int fixed);
	UNIGINE_INLINE int isFixed() const { return fixed; };

	UNIGINE_INLINE void setCollision(int collision) { this->collision = collision; }
	UNIGINE_INLINE int getCollision() const { return collision; }

	UNIGINE_INLINE void setCollisionMask(int mask) { shape->setCollisionMask(mask); };
	UNIGINE_INLINE int getCollisionMask() const { return shape->getCollisionMask(); };

	UNIGINE_INLINE void setCollisionRadius(float radius) { shape->setRadius(radius); };
	UNIGINE_INLINE float getCollisionRadius() const { return shape->getRadius(); };

	void setTarget(Unigine::NodePtr node);
	UNIGINE_INLINE Unigine::NodePtr getTarget() const { return target_node; };

	void setAnchor(const Unigine::Math::vec3& anchor);
	UNIGINE_INLINE const Unigine::Math::vec3& getAnchor() const { return anchor; };

	UNIGINE_INLINE void setMinDistance(float distance) { min_distance = Unigine::Math::max(distance, 0.0f); };
	UNIGINE_INLINE float getMinDistance() const { return min_distance; };

	UNIGINE_INLINE void setMaxDistance(float distance) { max_distance = Unigine::Math::max(distance, 0.0f); };
	UNIGINE_INLINE float getMaxDistance() const { return max_distance; };

	UNIGINE_INLINE void setMinThetaAngle(float angle) { min_theta_angle = Unigine::Math::clamp(angle, -PLAYER_PERSECUTOR_CLAMP, PLAYER_PERSECUTOR_CLAMP);};
	UNIGINE_INLINE float getMinThetaAngle() const { return min_theta_angle; };

	UNIGINE_INLINE void setMaxThetaAngle(float angle) { max_theta_angle = Unigine::Math::clamp(angle, -PLAYER_PERSECUTOR_CLAMP, PLAYER_PERSECUTOR_CLAMP); };
	UNIGINE_INLINE float getMaxThetaAngle() const { return max_theta_angle; };

	void setViewDirection(const Unigine::Math::vec3& view);
	UNIGINE_INLINE const Unigine::Math::vec3& getViewDirection() const { return direction; };

	UNIGINE_INLINE void setTurning(float turning) { this->turning = Unigine::Math::max(turning, 0.0f); };
	UNIGINE_INLINE float getTurning() const { return turning; };

	void setDistance(float distance);
	UNIGINE_INLINE float getDistance() const { return distance; };

	void setPhiAngle(float angle);
	float getPhiAngle() const;

	void setThetaAngle(float angle);
	float getThetaAngle() const;

	UNIGINE_INLINE void setView(const Unigine::Math::vec3& view);
	UNIGINE_INLINE const Unigine::Math::vec3& getView() const { return direction; };

	UNIGINE_INLINE int getNumContacts() const { return contacts.size(); };

	UNIGINE_INLINE const Unigine::ShapeContactPtr& getContact(int num) const;
	
	UNIGINE_INLINE float getContactDepth(int num) const;
	UNIGINE_INLINE Unigine::Math::vec3 getContactNormal(int num) const;
	UNIGINE_INLINE Unigine::ObjectPtr getContactObject(int num);
	UNIGINE_INLINE Unigine::Math::Vec3 getContactPoint(int num) const;
	UNIGINE_INLINE Unigine::ShapePtr getContactShape(int num) const;
	UNIGINE_INLINE int getContactSurface(int num) const;

	UNIGINE_INLINE void flushTransform();

private:
	void init();
	void update();
	void shutdown();

	void update_distance();
	void update_angles(float phi, float theta);
	void update_transform();


	Unigine::PlayerDummyPtr camera;
	Unigine::ShapeSpherePtr shape;

	float turning{0.0f};

	int flush{0};
	Unigine::Math::Vec3 target;
	Unigine::Math::Vec3 position;
	Unigine::Math::vec3 direction;
	float distance{0.0f};
	Unigine::Math::vec3 phi_direction;
	float theta_angle{0.0f};

	Unigine::Vector<Unigine::ShapeContactPtr> contacts;

	const float PLAYER_PERSECUTOR_CLAMP = 89.9f;
	const int PLAYER_PERSECUTOR_COLLISIONS = 4;
};